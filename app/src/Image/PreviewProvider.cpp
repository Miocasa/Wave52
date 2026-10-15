#include "PreviewProvider.h"

#include <QHash>
#include <QMutex>
#include <QMutexLocker>

namespace {
  QMutex g_mutex;
  QHash<QString, QImage> g_images;
} // namespace

void PreviewProvider::setImage(const QString& key, const QImage& image) {
  QMutexLocker lock(&g_mutex);
  g_images.insert(key, image);
}

QImage PreviewProvider::requestImage(const QString& id, QSize* size, const QSize&) {
  QImage img;
  {
    QMutexLocker lock(&g_mutex);
    img = g_images.value(id.section('?', 0, 0)); // cut off "?revision"
  }
  if (img.isNull()) {
    img = QImage(1, 1, QImage::Format_ARGB32);
    img.fill(Qt::transparent);
  }
  if (size) {
    *size = img.size();
  }
  return img;
}
