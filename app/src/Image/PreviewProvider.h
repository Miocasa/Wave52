#pragma once

#include <QImage>
#include <QQuickImageProvider>

class PreviewProvider : public QQuickImageProvider {
public:
  PreviewProvider() : QQuickImageProvider(QQuickImageProvider::Image) {}

  QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;
  static void setImage(const QString& key, const QImage& image);
};
