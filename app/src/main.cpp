#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "AppController.h"
#include "Image/PreviewProvider.h"
#ifdef Q_OS_ANDROID
#include <QBluetoothPermission>
#endif
#include <QQuickStyle>


int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  // QQuickStyle::setStyle("Material");
  // QQuickStyle::setStyle("Fusion");

  AppController controller;
  QQmlApplicationEngine engine;

#ifndef Q_OS_ANDROID
  if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORMTHEME"))
    qputenv("QT_QPA_PLATFORMTHEME", "xdgdesktopportal");
#endif
#ifdef QT_DEBUG
  engine.rootContext()->setContextProperty("isDebugBuild", true);
#else
  engine.rootContext()->setContextProperty("isDebugBuild", false);
#endif
#ifdef QT_QML_OUTPUT_DIRECTORY
  engine.addImportPath(QT_QML_OUTPUT_DIRECTORY);
#endif

  engine.addImageProvider("preview", new PreviewProvider);
  engine.rootContext()->setContextProperty("app", &controller);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app, [] { QCoreApplication::exit(-1); },
      Qt::QueuedConnection);

  engine.loadFromModule("Wave52", "Main");

#ifdef Q_OS_ANDROID
  QBluetoothPermission perm;
  perm.setCommunicationModes(QBluetoothPermission::Access);
  if (app.checkPermission(perm) == Qt::PermissionStatus::Undetermined) {
    app.requestPermission(perm, &controller,
                          [](const QPermission &p) { qDebug() << "Bluetooth permission:" << int(p.status()); });
  }
#endif


  return app.exec();
}
