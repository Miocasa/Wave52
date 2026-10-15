#include "BleClient.h"

#include <QElapsedTimer>
#include <QEventLoop>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyController>
#include <QLowEnergyDescriptor>
#include <QLowEnergyService>
#include <QTimer>
#include <QtBluetooth>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace {
  const QBluetoothUuid kService(QString::fromLatin1(Proto::kSvcConfig));
  const QBluetoothUuid kCmd(QString::fromLatin1(Proto::kChrCmd));
  const QBluetoothUuid kStatus(QString::fromLatin1(Proto::kChrSts));
  const QBluetoothUuid kData(QString::fromLatin1(Proto::kChrDat));

  // using Proto::Status;
  constexpr int kRefreshTimeoutMs = 30000; // display refresh timeout
  QString deviceKey(const QBluetoothDeviceInfo &i) {
    const QString a = i.address().toString();
    if (!a.isEmpty() && a != QString::fromLatin1("00:00:00:00:00:00")) {
      return a;
    }
    return i.deviceUuid().toString();
  }
  uint16_t crc16(const uint8_t *data, int len) {
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < len; ++i) {
      crc ^= data[i];
      for (int j = 0; j < 8; ++j) {
        crc = (crc & 1) ? static_cast<uint16_t>((crc >> 1) ^ 0xA001) : static_cast<uint16_t>(crc >> 1);
      }
    }
    return crc;
  }

  QByteArray buildChunk(uint8_t type, int num, int total, const uint8_t *payload, int len, bool last) {
    QByteArray c(Proto::kChunkHeader + len, 0);
    c[0] = static_cast<char>(type);
    c[1] = static_cast<char>(num & 0xFF);
    c[2] = static_cast<char>((total >> 8) & 0xFF);
    c[3] = static_cast<char>(total & 0xFF);
    c[4] = static_cast<char>(last ? (Proto::kFlagCrc | Proto::kFlagLast) : Proto::kFlagCrc);
    const uint16_t crc = crc16(payload, len);
    c[5] = static_cast<char>(crc >> 8);
    c[6] = static_cast<char>(crc & 0xFF);
    std::memcpy(c.data() + Proto::kChunkHeader, payload, len);
    return c;
  }
} // namespace


BleClient::BleClient(QObject *parent) : QObject(parent), ble_status_(std::make_unique<BleStatus>()) {}

BleClient::~BleClient() {
  resetLink();
}

void BleClient::setChunkPayload(int bytes) {
  payload_ = std::max(1, bytes);
}

int BleClient::chunkPayload() {
  return payload_.load();
}
struct BleClient::BleStatus {
  QBluetoothDeviceDiscoveryAgent *agent = nullptr;
  std::vector<QBluetoothDeviceInfo> devices;
  bool scanDone = false;
  QString scanError;
  // std::runtime_error& scanError;

  // link
  QLowEnergyController *ctrl = nullptr;
  QLowEnergyService *svc = nullptr;
  QLowEnergyCharacteristic cmdCh, statusCh, dataCh;
  bool linkUp = false;
  bool ctrlFailed = false;
  bool discovered = false;
  bool svcReady = false;
  bool linkReady = false;
  bool notifyOn = false;
  bool canReadStatus = false;
  bool dataNoResponse = false;

  // writes
  bool writeDone = false;
  bool writeFailed = false;
  QString writeError;

  std::atomic<int> status{-1};
};

void BleClient::scan() {
  emit busyChanged(true);
  try {
    if (!ble_status_->agent) {
      ble_status_->agent = new QBluetoothDeviceDiscoveryAgent(this);
      ble_status_->agent->setLowEnergyDiscoveryTimeout(3000);

      connect(ble_status_->agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this,
              [this](const QBluetoothDeviceInfo &info) {
                if (!(info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)) {
                  return;
                }
                if (info.name().isEmpty()) {
                  return;
                }
                const QString key = deviceKey(info);
                for (auto &e: ble_status_->devices) {
                  if (deviceKey(e) == key) {
                    e = info;
                    return;
                  }
                }
                ble_status_->devices.push_back(info);
              });
      connect(ble_status_->agent, &QBluetoothDeviceDiscoveryAgent::finished, this,
              [this] { ble_status_->scanDone = true; });
      connect(ble_status_->agent, &QBluetoothDeviceDiscoveryAgent::canceled, this,
              [this] { ble_status_->scanDone = true; });
      connect(ble_status_->agent, &QBluetoothDeviceDiscoveryAgent::errorOccurred, this,
              [this](QBluetoothDeviceDiscoveryAgent::Error) {
                ble_status_->scanError = ble_status_->agent->errorString();
                ble_status_->scanDone = true;
              });
    }

    ble_status_->devices.clear();
    ble_status_->scanDone = false;
    ble_status_->scanError.clear();

    emit log("Scanning (3s)...");
    ble_status_->agent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    if (!waitUntil([this] { return ble_status_->scanDone; }, 10000)) {
      ble_status_->agent->stop();
      throw std::runtime_error("scan timeout");
    }
    if (!ble_status_->scanError.isEmpty()) {
      throw std::runtime_error(ble_status_->scanError.toStdString());
    }

    QStringList names;
    int preferred = -1;
    for (size_t i = 0; i < ble_status_->devices.size(); i++) {
      const auto &dev = ble_status_->devices[i];
      names << QString("%1  [%2]").arg(dev.name(), deviceKey(dev));
      if (preferred < 0 && dev.name() == Proto::kTargetName) {
        preferred = static_cast<int>(i);
      }
    }

    emit log(QString("Devices found: %1").arg(names.size()));
    emit devicesFound(names, preferred);
  } catch (const std::exception &e) {
    emit log(QString("Scan error: ") + QString::fromUtf8(e.what()));
  }
  emit busyChanged(false);
}

void BleClient::connectTo(int index) {
  emit busyChanged(true);
  try {
    if (index < 0 || index >= static_cast<int>(ble_status_->devices.size())) {
      throw std::runtime_error("no device selected");
    }

    const QBluetoothDeviceInfo info = ble_status_->devices[index];
    emit log(QString("Connecting to %1...").arg(info.name()));

    resetLink();
    ble_status_->ctrl = QLowEnergyController::createCentral(info, this);

    connect(ble_status_->ctrl, &QLowEnergyController::connected, this, [this] { ble_status_->linkUp = true; });
    connect(ble_status_->ctrl, &QLowEnergyController::discoveryFinished, this,
            [this] { ble_status_->discovered = true; });
    connect(ble_status_->ctrl, &QLowEnergyController::errorOccurred, this, [this](QLowEnergyController::Error) {
      ble_status_->ctrlFailed = true;
      emit log("Controller error: " + ble_status_->ctrl->errorString());
    });
    connect(ble_status_->ctrl, &QLowEnergyController::disconnected, this, [this] {
      const bool was = ble_status_->linkReady;
      ble_status_->linkReady = false;
      ble_status_->linkUp = false;
      if (was) {
        emit log("Link lost");
        emit connectionChanged(false);
      }
    });

    bool connected = false;
    for (int attempt = 1; attempt <= 3 && !connected; ++attempt) {
      ble_status_->linkUp = false;
      ble_status_->ctrlFailed = false;
      ble_status_->ctrl->connectToDevice();
      connected =
          waitUntil([this] { return ble_status_->linkUp || ble_status_->ctrlFailed; }, 10000) && ble_status_->linkUp;
      if (!connected) {
        emit log(QString("Attempt %1 failed").arg(attempt));
        ble_status_->ctrl->disconnectFromDevice();
        pump(700);
      }
    }
    if (!connected) {
      throw std::runtime_error("failed to connect");
    }

    ble_status_->discovered = false;
    ble_status_->ctrl->discoverServices();
    if (!waitUntil([this] { return ble_status_->discovered || ble_status_->ctrlFailed; }, 15000) ||
        !ble_status_->discovered) {
      throw std::runtime_error("service discovery failed");
    }

    ble_status_->svc = ble_status_->ctrl->createServiceObject(kService, this);
    if (!ble_status_->svc) {
      throw std::runtime_error("Wave52 service not found on device");
    }

    BleStatus *impl = ble_status_.get();
    connect(ble_status_->svc, &QLowEnergyService::stateChanged, this, [impl](QLowEnergyService::ServiceState s) {
      if (s == QLowEnergyService::RemoteServiceDiscovered) {
        impl->svcReady = true;
      }
    });
    connect(ble_status_->svc, &QLowEnergyService::characteristicChanged, this,
            [impl](const QLowEnergyCharacteristic &c, const QByteArray &v) {
              if (c.uuid() == kStatus && !v.isEmpty()) {
                impl->status = static_cast<uint8_t>(v[0]);
              }
            });
    connect(ble_status_->svc, &QLowEnergyService::characteristicRead, this,
            [impl](const QLowEnergyCharacteristic &c, const QByteArray &v) {
              if (c.uuid() == kStatus && !v.isEmpty()) {
                impl->status = static_cast<uint8_t>(v[0]);
              }
            });
    connect(ble_status_->svc, &QLowEnergyService::characteristicWritten, this,
            [impl](const QLowEnergyCharacteristic &, const QByteArray &) { impl->writeDone = true; });
    connect(ble_status_->svc, &QLowEnergyService::errorOccurred, this, [this, impl](QLowEnergyService::ServiceError) {
      impl->writeFailed = true;
      impl->writeError = QString("GATT service error %1").arg(int(ble_status_->svc->error()));
    });

    ble_status_->svc->discoverDetails();
    if (!waitUntil([this] { return ble_status_->svcReady; }, 15000)) {
      throw std::runtime_error("service details discovery failed");
    }

    ble_status_->cmdCh = ble_status_->svc->characteristic(kCmd);
    ble_status_->statusCh = ble_status_->svc->characteristic(kStatus);
    ble_status_->dataCh = ble_status_->svc->characteristic(kData);
    if (!ble_status_->cmdCh.isValid() || !ble_status_->statusCh.isValid() || !ble_status_->dataCh.isValid()) {
      throw std::runtime_error("required characteristics not found");
    }

    ble_status_->canReadStatus = ble_status_->statusCh.properties() & QLowEnergyCharacteristic::Read;
    ble_status_->dataNoResponse = ble_status_->dataCh.properties() & QLowEnergyCharacteristic::WriteNoResponse;

    ble_status_->status = -1;
    ble_status_->notifyOn = false;
    const QLowEnergyDescriptor cccd =
        ble_status_->statusCh.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
    if ((ble_status_->statusCh.properties() & QLowEnergyCharacteristic::Notify) && cccd.isValid()) {
      ble_status_->svc->writeDescriptor(cccd, QByteArray::fromHex("0100"));
      pump(200);
      ble_status_->notifyOn = true;
    } else {
      emit log("Status notifications unavailable, polling");
    }

    ble_status_->linkReady = true;
    emit log(QString("MTU: %1, data write: %2")
                 .arg(ble_status_->ctrl->mtu())
                 .arg(ble_status_->dataNoResponse ? "without response" : "with response"));
    emit log("Connected");
    emit connectionChanged(true);
  } catch (const std::exception &e) {
    emit log(QString("Connection error: ") + QString::fromUtf8(e.what()));
    resetLink();
    emit connectionChanged(false);
  }
  emit busyChanged(false);
}

void BleClient::disconnectDevice() {
  emit busyChanged(true);
  try {
    if (ble_status_->ctrl) {
      QObject::disconnect(ble_status_->ctrl, nullptr, this, nullptr);
      ble_status_->ctrl->disconnectFromDevice();
      waitUntil([this] { return ble_status_->ctrl->state() == QLowEnergyController::UnconnectedState; }, 3000);
    }
  } catch (const std::exception &e) {
    emit log(QString("Disconnect error: ") + QString::fromUtf8(e.what()));
  }
  resetLink();
  emit log("Disconnected");
  emit connectionChanged(false);
  emit busyChanged(false);
}

void BleClient::pump(int ms) {
  QEventLoop loop;
  QTimer::singleShot(ms, &loop, &QEventLoop::quit);
  loop.exec();
}

bool BleClient::waitUntil(const std::function<bool()> &pred, int timeoutMs) {
  QElapsedTimer t;
  t.start();
  while (!pred()) {
    if (t.elapsed() >= timeoutMs) {
      return false;
    }
    pump(10);
  }
  return true;
}
void BleClient::resetLink() {
  if (ble_status_->svc) {
    QObject::disconnect(ble_status_->svc, nullptr, this, nullptr);
    delete ble_status_->svc;
    ble_status_->svc = nullptr;
  }
  if (ble_status_->ctrl) {
    QObject::disconnect(ble_status_->ctrl, nullptr, this, nullptr);
    if (ble_status_->ctrl->state() != QLowEnergyController::UnconnectedState) {
      ble_status_->ctrl->disconnectFromDevice();
    }
    delete ble_status_->ctrl;
    ble_status_->ctrl = nullptr;
  }
  ble_status_->cmdCh = ble_status_->statusCh = ble_status_->dataCh = QLowEnergyCharacteristic();
  ble_status_->linkUp = ble_status_->ctrlFailed = ble_status_->discovered = ble_status_->svcReady = false;
  ble_status_->linkReady = ble_status_->notifyOn = ble_status_->canReadStatus = ble_status_->dataNoResponse = false;
}
void BleClient::checkCancelled() const {
  if (cancel_.load()) {
    throw std::runtime_error("cancelled");
  }
}

void BleClient::requireConnected() {
  if (!ble_status_->ctrl || !ble_status_->svc || !ble_status_->linkReady) {
    emit connectionChanged(false);
    throw std::runtime_error("not connected to the device");
  }
}

void BleClient::writeChar(bool data, const QByteArray &value, bool withResponse) {
  const QLowEnergyCharacteristic &ch = data ? ble_status_->dataCh : ble_status_->cmdCh;
  ble_status_->writeDone = false;
  ble_status_->writeFailed = false;
  ble_status_->writeError.clear();

  ble_status_->svc->writeCharacteristic(
      ch, value, withResponse ? QLowEnergyService::WriteWithResponse : QLowEnergyService::WriteWithoutResponse);

  if (withResponse) {
    if (!waitUntil([this] { return ble_status_->writeDone || ble_status_->writeFailed; }, 5000)) {
      throw std::runtime_error("write timed out");
    }
    if (ble_status_->writeFailed) {
      throw std::runtime_error(ble_status_->writeError.toStdString());
    }
  } else {
    pump(5);
    if (ble_status_->writeFailed) {
      throw std::runtime_error(ble_status_->writeError.toStdString());
    }
    if (!ble_status_->linkReady) {
      throw std::runtime_error("connection lost");
    }
  }
}

void BleClient::writeCommand(uint8_t cmd, uint8_t param) {
  QByteArray bytes;
  bytes.append(static_cast<char>(cmd));
  bytes.append(static_cast<char>(param));
  writeChar(false, bytes, true);
}

int BleClient::effectivePayload() const {
  int p = payload_.load();
  if (ble_status_->ctrl) {
    const int mtu = ble_status_->ctrl->mtu();
    if (mtu > 0) {
      p = std::min(p, mtu - 3 - Proto::kChunkHeader);
    }
  }
  return p;
}

bool BleClient::waitForStatus(int expected, int timeoutMs) {
  QElapsedTimer timer;
  timer.start();
  int iter = 0;

  while (timer.elapsed() < timeoutMs) {
    checkCancelled();
    const int s = ble_status_->status.load();
    if (s == expected) {
      return true;
    }
    if (s == Proto::kStatusError) {
      return false;
    }
    if (!ble_status_->linkReady) {
      throw std::runtime_error("connection lost");
    }

    if (ble_status_->canReadStatus && (!ble_status_->notifyOn || iter % 10 == 0)) {
      ble_status_->svc->readCharacteristic(ble_status_->statusCh);
    }
    ++iter;
    pump(50);
  }
  return false;
}

void BleClient::commandAndWait(uint8_t cmd, uint8_t param, int timeoutMs, const QString &what) {
  ble_status_->status = -1;
  writeCommand(cmd, param);
  pump(150);
  if (!waitForStatus(Proto::kStatusSuccess, timeoutMs)) {
    throw std::runtime_error(QString("%1 failed").arg(what).toStdString());
  }
}

void BleClient::refreshDisplay() {
  emit log("Refreshing display...");
  commandAndWait(Proto::kCmdRefreshDisplay, 0, kRefreshTimeoutMs, "display refresh");
  emit log("Display refreshed");
}

void BleClient::transferChunks(uint8_t cmd, uint8_t param, uint8_t type, const QByteArray &data, const QString &label) {
  const int payload = effectivePayload();
  if (payload < 1) {
    throw std::runtime_error("MTU too small for the chunk header");
  }

  const int total = (static_cast<int>(data.size()) + payload - 1) / payload;
  if (total == 0 || total > Proto::kMaxChunks) {
    throw std::runtime_error(QString("invalid data size (%1 chunks of %2 bytes, max %3)")
                                 .arg(total)
                                 .arg(payload)
                                 .arg(Proto::kMaxChunks)
                                 .toStdString());
  }

  emit log(QString("Sending %1: %2 bytes, %3 chunks of %4").arg(label).arg(data.size()).arg(total).arg(payload));

  ble_status_->status = -1;
  writeCommand(cmd, param);
  pump(150);
  if (!waitForStatus(Proto::kStatusWaiting, 5000)) {
    throw std::runtime_error("device is not ready to receive");
  }

  ble_status_->status = -1;
  const uint8_t *raw = reinterpret_cast<const uint8_t *>(data.constData());
  const bool withResponse = !ble_status_->dataNoResponse;

  for (int i = 0; i < total; ++i) {
    checkCancelled();
    if (ble_status_->status.load() == Proto::kStatusError) {
      throw std::runtime_error("device rejected the data (check chunk type, CRC and layout)");
    }

    const int offset = i * payload;
    const int len = std::min(payload, static_cast<int>(data.size()) - offset);
    const QByteArray chunk = buildChunk(type, i, total, raw + offset, len, i == total - 1);
    writeChar(true, chunk, withResponse);
    emit progress(i + 1, total);
  }

  if (!waitForStatus(Proto::kStatusSuccess, 10000)) {
    throw std::runtime_error(QString("the %1 was rejected or not confirmed by the device").arg(label).toStdString());
  }
  emit log(label + " received");
}

void BleClient::runTransfer(uint8_t cmd, uint8_t param, uint8_t type, const QByteArray &data, const QString &label,
                            bool refreshAfter) {
  beginOperation();
  emit busyChanged(true);
  bool ok = false;
  try {
    requireConnected();
    transferChunks(cmd, param, type, data, label);
    if (refreshAfter) {
      refreshDisplay();
    }
    ok = true;
  } catch (const std::exception &e) {
    emit log(QString("Transfer error: ") + QString::fromUtf8(e.what()));
  }
  emit transferFinished(ok);
  emit busyChanged(false);
}

void BleClient::sendCommand(int cmd, int param, const QString &label, int timeoutMs) {
  beginOperation();
  emit busyChanged(true);
  try {
    requireConnected();
    ble_status_->status = -1;
    writeCommand(static_cast<uint8_t>(cmd), static_cast<uint8_t>(param));
    pump(150);
    if (!waitForStatus(Proto::kStatusSuccess, timeoutMs)) {
      throw std::runtime_error("device returned an error or did not respond");
    }
    emit log(label + ": done");
  } catch (const std::exception &e) {
    emit log(label + " failed: " + QString::fromUtf8(e.what()));
  }
  emit busyChanged(false);
}

void BleClient::sendBackground(const QByteArray &data, bool refreshAfter) {
  runTransfer(Proto::kCmdRecvBackground, 0, static_cast<uint8_t>(Proto::WidgetTypes::kTypeBackground), data,
              "background", refreshAfter);
}

void BleClient::sendWidget(int index, int type, const QByteArray &data, bool refreshAfter) {
  runTransfer(Proto::kCmdRecvWidget, static_cast<uint8_t>(index), static_cast<uint8_t>(type), data, "widget",
              refreshAfter);
}

void BleClient::sendScene(const QByteArray &background, const QList<QByteArray> &widgets, bool refreshAfter) {
  beginOperation();
  emit busyChanged(true);
  bool ok = false;
  try {
    requireConnected();

    emit log("Clearing widgets...");
    commandAndWait(Proto::kCmdClearWidgets, 0, 5000, "clear widgets");

    if (!background.isEmpty()) {
      transferChunks(Proto::kCmdRecvBackground, 0, static_cast<uint8_t>(Proto::WidgetTypes::kTypeBackground),
                     background, "background");
    }

    for (int i = 0; i < widgets.size(); ++i) {
      transferChunks(Proto::kCmdRecvWidget, Proto::kAppendIndex, static_cast<uint8_t>(Proto::WidgetTypes::kTypeTextBox),
                     widgets[i], QString("widget %1/%2").arg(i + 1).arg(widgets.size()));
    }

    if (refreshAfter) {
      refreshDisplay();
    }

    emit log("Scene sent");
    ok = true;
  } catch (const std::exception &e) {
    emit log(QString("Scene error: ") + QString::fromUtf8(e.what()));
  }
  emit transferFinished(ok);
  emit busyChanged(false);
}
