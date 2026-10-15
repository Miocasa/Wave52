#pragma once
#include <QByteArray>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <atomic>
#include <functional>
#include <memory>

#include "Protocol.h"

class BleClient : public QObject {
  Q_OBJECT

public:
  BleClient(QObject* parent = nullptr);
  ~BleClient() override;

  void setChunkPayload(int payload);
  int chunkPayload();
  void cancel() {
    cancel_ = true;
  }

public slots:
  void scan();
  void connectTo(int index);
  void disconnectDevice();
  void sendCommand(int cmd, int param, const QString& label, int timeoutMs);

  void sendBackground(const QByteArray& data, bool refreshAfter);
  void sendWidget(int index, int type, const QByteArray& data, bool refreshAfter);
  void sendScene(const QByteArray& background, const QList<QByteArray>& widgets, bool refreshAfter);

signals:
  void log(const QString& text);
  void devicesFound(const QStringList& names, int preferred);
  void connectionChanged(bool connected);
  void progress(int done, int total);
  void busyChanged(bool busy);
  void transferFinished(bool ok);

private:
  struct BleStatus;

  void runTransfer(uint8_t cmd, uint8_t param, uint8_t type, const QByteArray& data, const QString& label,
                   bool refreshAfter);
  void transferChunks(uint8_t cmd, uint8_t param, uint8_t type, const QByteArray& data, const QString& label);
  void commandAndWait(uint8_t cmd, uint8_t param, int timeoutMs, const QString& what);
  void refreshDisplay();
  bool waitForStatus(int expected, int timeoutMs);
  void writeCommand(uint8_t cmd, uint8_t param);
  void writeChar(bool data, const QByteArray& value, bool withResponse);
  int effectivePayload() const;
  void requireConnected();
  void beginOperation() {
    cancel_ = false;
  }
  void checkCancelled() const;

  void pump(int ms);
  bool waitUntil(const std::function<bool()>& pred, int timeoutMs);
  void resetLink();

  std::unique_ptr<BleStatus> ble_status_;
  std::atomic<int> payload_{Proto::kChunkPayload};
  std::atomic<bool> cancel_{false};
};
