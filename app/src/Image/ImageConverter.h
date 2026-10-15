#pragma once

#include <QByteArray>
#include <QImage>

class ImageConverter
{
public:
    enum class Fit { Cover = 0, Contain = 1, Stretch = 2 };

    struct Options
    {
        int width = 400;
        int height = 300;
        Fit fit = Fit::Cover;
        int rotation = 0;
        int brightness = 0;
        int contrast = 0;
        int background = 255;
        bool invert = false;
        bool dither = false;
    };

    static QImage convert(const QImage& source, const Options& options);
    static QByteArray pack(const QImage& quantized);
};
