#include "AppController.h"

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>
#include <QTransform>

#include "BleClient.h"
#include "Image/PreviewProvider.h"
#include "Image/ScenePreview.h"
#include "Models/WidgetSerializer.h"
#include "Protocol.h"
#include "Widgets/FontManager.h"

namespace {
  QString urlToPath(const QUrl& url) {
    // Android 'content://' header
    return url.isLocalFile() ? url.toLocalFile() : url.toString();
  }
} // namespace

AppController::AppController(QObject* parent) : QObject(parent), widgets_(new WidgetListModel(this)) {
  previewTimer_.setSingleShot(true);
  previewTimer_.setInterval(20);
  connect(&previewTimer_, &QTimer::timeout, this, &AppController::updatePreview);
  connect(this, &AppController::settingsChanged, &previewTimer_, qOverload<>(&QTimer::start));
  connect(widgets_, &WidgetListModel::specsChanged, this, &AppController::updateScreen);

  startWorker();
  appendLog("Drag and drop an image or click «Open»");
  updatePreview();
}

AppController::~AppController() {
  if (thread_.isRunning()) {
    BleClient* ble = ble_;
    QMetaObject::invokeMethod(ble, [ble] { ble->disconnectDevice(); }, Qt::BlockingQueuedConnection);
    thread_.quit();
    thread_.wait();
  }
}

void AppController::startWorker() {
  ble_ = new BleClient;
  ble_->moveToThread(&thread_);
  connect(&thread_, &QThread::finished, ble_, &QObject::deleteLater);

  connect(ble_, &BleClient::log, this, &AppController::appendLog);
  connect(ble_, &BleClient::devicesFound, this, &AppController::onDevicesFound);
  connect(ble_, &BleClient::connectionChanged, this, &AppController::onConnectionChanged);
  connect(ble_, &BleClient::progress, this, &AppController::onProgress);
  connect(ble_, &BleClient::busyChanged, this, &AppController::onBusyChanged);
  connect(ble_, &BleClient::transferFinished, this, &AppController::onTransferFinished);

  thread_.start();
}

int AppController::maxTextBytes() const {
  return Layout::kMaxTextBytes;
}

QStringList AppController::fontNames() const {
  QStringList names;
  for (int i = 0; i < FontMgr::count(); ++i) {
    names << FontMgr::kFonts[i].name;
  }
  return names;
}

bool AppController::canSendBackground() const {
  if (!ready() || packed_.isEmpty()) {
    return false;
  }
  const int chunks = (static_cast<int>(packed_.size()) + Proto::kChunkPayload - 1) / Proto::kChunkPayload;
  return chunks <= Proto::kMaxChunks;
}

ImageConverter::Options AppController::options() const {
  ImageConverter::Options o;
  o.width = imgWidth_;
  o.height = imgHeight_;
  o.fit = static_cast<ImageConverter::Fit>(fit_);
  o.rotation = rotation_;
  o.brightness = brightness_;
  o.contrast = contrast_;
  o.background = background_;
  o.invert = invert_;
  o.dither = dither_;
  return o;
}

// Preview
void AppController::updatePreview() {
  if (source_.isNull()) {
    packed_.clear();
  } else {
    packed_ = ImageConverter::pack(ImageConverter::convert(source_, options()));
  }
  updateScreen();
}

void AppController::updateScreen() {
  const auto opts = options();

  const int angle = opts.rotation;
  const bool swap = (angle == 90 || angle == 270);
  const int actualAngle = swap ? (angle == 90 ? 270 : 90) : angle;

  screen_ = ScenePreview::render(packed_, opts.width, opts.height, widgets_->specs());


  PreviewProvider::setImage("screen", screen_); // screen_.transformed(QTransform().rotate(actualAngle))
  ++screenRevision_;

  const int bytes = ((opts.width + 3) / 4) * opts.height;
  const int chunks = (bytes + Proto::kChunkPayload - 1) / Proto::kChunkPayload;

  QString text = QString("%1×%2 · %3 bytes · %4 chunks · %5 widgets")
                     .arg(opts.width)
                     .arg(opts.height)
                     .arg(bytes)
                     .arg(chunks)
                     .arg(widgets_->count());
  if (bytes != Proto::kBackgroundBytes) {
    text += QString("\nFirmware expects %1 bytes (400×300)").arg(Proto::kBackgroundBytes);
  }
  if (chunks > Proto::kMaxChunks) {
    text += "\nBitmap is too large for the protocol";
  }
  infoText_ = text;

  emit previewChanged();
  emit stateChanged();
}

// Image

void AppController::loadImage(const QUrl& url) {
  QFile file(urlToPath(url));
  if (!file.open(QIODevice::ReadOnly)) {
    emit errorOccurred("Failed to open image:\n" + file.errorString());
    return;
  }

  QImageReader reader(&file);
  reader.setAutoTransform(true);
  const QImage image = reader.read();
  if (image.isNull()) {
    emit errorOccurred("Failed to open image:\n" + reader.errorString());
    return;
  }

  source_ = image;
  PreviewProvider::setImage("original", image);
  ++originalRevision_;
  fileName_ = url.isLocalFile() ? QFileInfo(url.toLocalFile()).fileName() : url.fileName();

  appendLog(QString("Loaded: %1 (%2×%3)").arg(fileName_).arg(image.width()).arg(image.height()));
  updatePreview(); // emit previewChanged and stateChanged
}

void AppController::saveBin(const QUrl& url) {
  if (packed_.isEmpty()) {
    return;
  }
  QFile file(urlToPath(url));
  if (!file.open(QIODevice::WriteOnly) || file.write(packed_) != packed_.size()) {
    emit errorOccurred("Failed to write file");
    return;
  }
  appendLog(QString("Saved %1 bytes to %2").arg(packed_.size()).arg(url.toDisplayString()));
}

void AppController::savePng(const QUrl& url) {
  if (screen_.isNull()) {
    return;
  }
  QFile file(urlToPath(url));
  if (!file.open(QIODevice::WriteOnly) || !screen_.save(&file, "PNG")) {
    emit errorOccurred("Failed to save image");
    return;
  }
  appendLog("Preview saved: " + url.toDisplayString());
}

void AppController::resetAdjustments() {
  brightness_ = 0;
  contrast_ = 0;
  background_ = 255;
  rotation_ = 0;
  fit_ = 0;
  dither_ = false;
  invert_ = false;
  imgWidth_ = 400;
  imgHeight_ = 300;
  emit settingsChanged();
}

// BLE

void AppController::scan() {
  BleClient* ble = ble_;
  QMetaObject::invokeMethod(ble, [ble] { ble->scan(); }, Qt::QueuedConnection);
}

void AppController::toggleConnection(int deviceIndex) {
  BleClient* ble = ble_;
  if (connected_) {
    QMetaObject::invokeMethod(ble, [ble] { ble->disconnectDevice(); }, Qt::QueuedConnection);
    return;
  }
  if (deviceIndex < 0) {
    return;
  }
  QMetaObject::invokeMethod(ble, [ble, deviceIndex] { ble->connectTo(deviceIndex); }, Qt::QueuedConnection);
}

void AppController::runCommand(int cmd, int param, const QString& label, int timeoutMs) {
  BleClient* ble = ble_;
  QMetaObject::invokeMethod(
      ble, [ble, cmd, param, label, timeoutMs] { ble->sendCommand(cmd, param, label, timeoutMs); },
      Qt::QueuedConnection);
}

void AppController::refreshDisplay(bool partial) {
  runCommand(Proto::kCmdRefreshDisplay, partial ? 1 : 0, partial ? "Partial refresh" : "Refresh", 30000);
}
void AppController::clearWidgets() {
  runCommand(Proto::kCmdClearWidgets, 0, "Clear widgets", 5000);
}
void AppController::clearBackground() {
  runCommand(Proto::kCmdClearBackground, 0xFF, "Clear background", 5000);
}
void AppController::saveScreen(int slot) {
  runCommand(Proto::kCmdSaveScreen, slot, "Save screen", 15000);
}
void AppController::loadScreen(int slot) {
  runCommand(Proto::kCmdLoadScreen, slot, "Load screen", 15000);
}
void AppController::loadDefaultScreen() {
  runCommand(Proto::kCmdLoadDefaultScreen, 0, "Load default screen", 30000);
}

void AppController::sendBackground() {
  if (packed_.isEmpty()) {
    return;
  }
  progressMax_ = 100;
  progressValue_ = 0;
  emit progressChanged();

  BleClient* ble = ble_;
  const QByteArray data = packed_;
  const bool refresh = refreshAfter_;
  QMetaObject::invokeMethod(ble, [ble, data, refresh] { ble->sendBackground(data, refresh); }, Qt::QueuedConnection);
}

void AppController::sendScene() {
  QList<QByteArray> payloads;
  for (const TextBoxSpec& spec: widgets_->specs()) {
    const QByteArray payload = WidgetSerializer::textBox(spec);
    if (payload.isEmpty()) {
      emit errorOccurred("One of the widgets has invalid data");
      return;
    }
    payloads.append(payload);
  }

  const QByteArray background = (includeBackground_ && !packed_.isEmpty()) ? packed_ : QByteArray();
  const bool refresh = refreshAfter_;

  progressMax_ = 100;
  progressValue_ = 0;
  emit progressChanged();

  BleClient* ble = ble_;
  QMetaObject::invokeMethod(
      ble, [ble, background, payloads, refresh] { ble->sendScene(background, payloads, refresh); },
      Qt::QueuedConnection);
}

// slots of BleClient

void AppController::appendLog(const QString& text) {
  logLines_ << QDateTime::currentDateTime().toString("HH:mm:ss  ") + text;
  while (logLines_.size() > 300) {
    logLines_.removeFirst();
  }
  logText_ = logLines_.join('\n');
  emit logChanged();
}

void AppController::onDevicesFound(const QStringList& names, int preferred) {
  deviceNames_ = names;
  preferred_ = preferred;
  emit devicesChanged();
  emit stateChanged();
}

void AppController::onConnectionChanged(bool connected) {
  connected_ = connected;
  emit stateChanged();
}

void AppController::onProgress(int done, int total) {
  progressMax_ = total;
  progressValue_ = done;
  emit progressChanged();
}

void AppController::onBusyChanged(bool busy) {
  busy_ = busy;
  emit stateChanged();
}

void AppController::onTransferFinished(bool ok) {
  if (!ok) {
    progressValue_ = 0;
    emit progressChanged();
  }
}
