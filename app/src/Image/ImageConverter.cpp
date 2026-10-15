#include "ImageConverter.h"

#include <QColor>
#include <QPainter>
#include <QTransform>

#include <algorithm>
#include <cmath>
#include <vector>

namespace {

constexpr float kLevelStep = 85.0f;

QImage flatten(const QImage& source)
{
    QImage canvas(source.size(), QImage::Format_RGB32);
    canvas.fill(Qt::white);
    QPainter painter(&canvas);
    painter.drawImage(0, 0, source);
    painter.end();
    return canvas;
}

QImage fitImage(const QImage& source, int w, int h, ImageConverter::Fit fit, int background)
{
    if (fit == ImageConverter::Fit::Stretch) {
        return source.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    if (fit == ImageConverter::Fit::Cover) {
        const QImage scaled = source.scaled(w, h, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        const int x = (scaled.width() - w) / 2;
        const int y = (scaled.height() - h) / 2;
        return scaled.copy(x, y, w, h);
    }

    const QImage scaled = source.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QImage canvas(w, h, QImage::Format_RGB32);
    canvas.fill(QColor(background, background, background));
    QPainter painter(&canvas);
    painter.drawImage((w - scaled.width()) / 2, (h - scaled.height()) / 2, scaled);
    painter.end();
    return canvas;
}

}

QImage ImageConverter::convert(const QImage& source, const Options& o)
{
    if (source.isNull() || o.width <= 0 || o.height <= 0) {
        return {};
    }

    QImage image = flatten(source);

    if (o.rotation % 360 != 0) {
        image = image.transformed(QTransform().rotate(o.rotation), Qt::SmoothTransformation);
    }

    const QImage gray = fitImage(image, o.width, o.height, o.fit, o.background)
                            .convertToFormat(QImage::Format_Grayscale8);

    const int w = o.width;
    const int h = o.height;
    const float factor = (100.0f + o.contrast) / 100.0f;
    const float shift = o.brightness * 2.55f;

    std::vector<float> buffer(static_cast<size_t>(w) * h);
    for (int y = 0; y < h; ++y) {
        const uchar* line = gray.constScanLine(y);
        for (int x = 0; x < w; ++x) {
            float v = (line[x] - 128.0f) * factor + 128.0f + shift;
            v = std::clamp(v, 0.0f, 255.0f);
            if (o.invert) {
                v = 255.0f - v;
            }
            buffer[static_cast<size_t>(y) * w + x] = v;
        }
    }

    QImage out(w, h, QImage::Format_Grayscale8);

    if (o.dither) {
        for (int y = 0; y < h; ++y) {
            uchar* line = out.scanLine(y);
            for (int x = 0; x < w; ++x) {
                const size_t i = static_cast<size_t>(y) * w + x;
                const float old = buffer[i];
                const int level = std::clamp(static_cast<int>(std::lround(old / kLevelStep)), 0, 3);
                const float err = old - level * kLevelStep;
                line[x] = static_cast<uchar>(level * 85);

                if (x + 1 < w) {
                    buffer[i + 1] += err * 7.0f / 16.0f;
                }
                if (y + 1 < h) {
                    const size_t next = i + w;
                    if (x > 0) {
                        buffer[next - 1] += err * 3.0f / 16.0f;
                    }
                    buffer[next] += err * 5.0f / 16.0f;
                    if (x + 1 < w) {
                        buffer[next + 1] += err / 16.0f;
                    }
                }
            }
        }
    } else {
        for (int y = 0; y < h; ++y) {
            uchar* line = out.scanLine(y);
            for (int x = 0; x < w; ++x) {
                const int level = std::clamp(static_cast<int>(buffer[static_cast<size_t>(y) * w + x]) >> 6, 0, 3);
                line[x] = static_cast<uchar>(level * 85);
            }
        }
    }

    return out;
}

QByteArray ImageConverter::pack(const QImage& quantized)
{
    if (quantized.isNull()) {
        return {};
    }

    const int w = quantized.width();
    const int h = quantized.height();
    const int byteWidth = (w + 3) / 4;

    QByteArray data(byteWidth * h, 0);
    uchar* raw = reinterpret_cast<uchar*>(data.data());

    for (int y = 0; y < h; ++y) {
        const uchar* line = quantized.constScanLine(y);
        for (int x = 0; x < w; ++x) {
            const int level = std::min(3, line[x] / 85);
            const int shift = (3 - (x % 4)) * 2;
            raw[y * byteWidth + x / 4] |= static_cast<uchar>(level << shift);
        }
    }

    return data;
}
