#pragma once

#include <QByteArray>
#include <QImage>
#include <QObject>
#include <QStringList>
#include <QThread>
#include <QTimer>
#include <QUrl>

#include "Image/ImageConverter.h"
#include "Models/WidgetListModel.h"

class BleClient;

class AppController : public QObject {
  Q_OBJECT

  // Device
  Q_PROPERTY(QStringList deviceNames READ deviceNames NOTIFY devicesChanged)
  Q_PROPERTY(int preferredDevice READ preferredDevice NOTIFY devicesChanged)

  // runtime states
  Q_PROPERTY(bool connected READ connected NOTIFY stateChanged)
  Q_PROPERTY(bool busy READ busy NOTIFY stateChanged)
  Q_PROPERTY(bool ready READ ready NOTIFY stateChanged)
  Q_PROPERTY(bool hasImage READ hasImage NOTIFY stateChanged)
  Q_PROPERTY(bool hasScreen READ hasScreen NOTIFY stateChanged)
  Q_PROPERTY(bool canSendBackground READ canSendBackground NOTIFY stateChanged)
  Q_PROPERTY(QString fileName READ fileName NOTIFY stateChanged)
  Q_PROPERTY(QString infoText READ infoText NOTIFY stateChanged)

  // Preview
  Q_PROPERTY(int originalRevision READ originalRevision NOTIFY previewChanged)
  Q_PROPERTY(int screenRevision READ screenRevision NOTIFY previewChanged)

  // log, progress
  Q_PROPERTY(QString logText READ logText NOTIFY logChanged)
  Q_PROPERTY(int progressValue READ progressValue NOTIFY progressChanged)
  Q_PROPERTY(int progressMax READ progressMax NOTIFY progressChanged)

  // widgets
  Q_PROPERTY(WidgetListModel* widgets READ widgets CONSTANT)
  Q_PROPERTY(int maxTextBytes READ maxTextBytes CONSTANT)
  Q_PROPERTY(QStringList fontNames READ fontNames CONSTANT)

  // Conversation params
  Q_PROPERTY(int fit MEMBER fit_ NOTIFY settingsChanged) // 0 Cover, 1 Contain, 2 Stretch
  Q_PROPERTY(int imgWidth MEMBER imgWidth_ NOTIFY settingsChanged)
  Q_PROPERTY(int imgHeight MEMBER imgHeight_ NOTIFY settingsChanged)
  Q_PROPERTY(int rotation MEMBER rotation_ NOTIFY settingsChanged) // 0/90/180/270
  Q_PROPERTY(int brightness MEMBER brightness_ NOTIFY settingsChanged)
  Q_PROPERTY(int contrast MEMBER contrast_ NOTIFY settingsChanged)
  Q_PROPERTY(int background MEMBER background_ NOTIFY settingsChanged)
  Q_PROPERTY(bool invert MEMBER invert_ NOTIFY settingsChanged)
  Q_PROPERTY(bool dither MEMBER dither_ NOTIFY settingsChanged)

  // send flags
  Q_PROPERTY(bool includeBackground MEMBER includeBackground_ NOTIFY flagsChanged)
  Q_PROPERTY(bool refreshAfter MEMBER refreshAfter_ NOTIFY flagsChanged)

public:
  explicit AppController(QObject* parent = nullptr);
  ~AppController() override;

  QStringList deviceNames() const {
    return deviceNames_;
  }
  int preferredDevice() const {
    return preferred_;
  }
  bool connected() const {
    return connected_;
  }
  bool busy() const {
    return busy_;
  }
  bool ready() const {
    return connected_ && !busy_;
  }
  bool hasImage() const {
    return !packed_.isEmpty();
  }
  bool hasScreen() const {
    return !screen_.isNull();
  }
  bool canSendBackground() const;
  QString fileName() const {
    return fileName_;
  }
  QString infoText() const {
    return infoText_;
  }
  int originalRevision() const {
    return originalRevision_;
  }
  int screenRevision() const {
    return screenRevision_;
  }
  QString logText() const {
    return logText_;
  }
  int progressValue() const {
    return progressValue_;
  }
  int progressMax() const {
    return progressMax_;
  }
  WidgetListModel* widgets() const {
    return widgets_;
  }
  int maxTextBytes() const;
  QStringList fontNames() const;

  Q_INVOKABLE int utf8Size(const QString& text) const {
    return text.toUtf8().size();
  }

  // Device
  Q_INVOKABLE void scan();
  Q_INVOKABLE void toggleConnection(int deviceIndex);

  // Image
  Q_INVOKABLE void loadImage(const QUrl& url);
  Q_INVOKABLE void saveBin(const QUrl& url);
  Q_INVOKABLE void savePng(const QUrl& url);
  Q_INVOKABLE void resetAdjustments();

  // Send
  Q_INVOKABLE void sendBackground();
  Q_INVOKABLE void sendScene();

  // commands
  Q_INVOKABLE void refreshDisplay(bool partial);
  Q_INVOKABLE void clearWidgets();
  Q_INVOKABLE void clearBackground();
  Q_INVOKABLE void saveScreen(int slot);
  Q_INVOKABLE void loadScreen(int slot);
  Q_INVOKABLE void loadDefaultScreen();

signals:
  void devicesChanged();
  void stateChanged();
  void previewChanged();
  void logChanged();
  void progressChanged();
  void settingsChanged();
  void flagsChanged();
  void errorOccurred(const QString& message);

private slots:
  void appendLog(const QString& text);
  void onDevicesFound(const QStringList& names, int preferred);
  void onConnectionChanged(bool connected);
  void onProgress(int done, int total);
  void onBusyChanged(bool busy);
  void onTransferFinished(bool ok);
  void updatePreview();
  void updateScreen();

private:
  void startWorker();
  void runCommand(int cmd, int param, const QString& label, int timeoutMs);
  ImageConverter::Options options() const;

  QThread thread_;
  BleClient* ble_ = nullptr;
  QTimer previewTimer_;

  // QString source_;
  QImage source_;
  QImage screen_;
  QByteArray packed_;
  WidgetListModel* widgets_ = nullptr;

  bool connected_ = false;
  bool busy_ = false;
  QStringList deviceNames_;
  int preferred_ = -1;

  QString fileName_ = QStringLiteral("No file selected");
  QString infoText_;
  int originalRevision_ = 0;
  int screenRevision_ = 0;

  QStringList logLines_;
  QString logText_;
  int progressValue_ = 0;
  int progressMax_ = 100;

  // Fit fit_ = 0;
  int fit_ = 0;
  int imgWidth_ = 400;
  int imgHeight_ = 300;
  // Rotation rotation_ = 0;
  int rotation_ = 0;
  int brightness_ = 0;
  int contrast_ = 0;
  int background_ = 255;
  bool invert_ = false;
  bool dither_ = false;

  bool includeBackground_ = true;
  bool refreshAfter_ = true;
};
