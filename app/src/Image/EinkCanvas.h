#pragma once

#include <Adafruit_GFX.h>
#ifdef F
#undef F
#endif

#include <QByteArray>
#include <QImage>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

#ifndef GxEPD_BLACK
#define GxEPD_BLACK     0x0000
#define GxEPD_WHITE     0xFFFF
#define GxEPD_DARKGREY  0x7BEF
#define GxEPD_LIGHTGREY 0xC618
#endif

class EinkCanvas : public Adafruit_GFX
{
public:
    static constexpr int16_t kWidth  = 400;
    static constexpr int16_t kHeight = 300;

    explicit EinkCanvas(int16_t w = kWidth, int16_t h = kHeight)
        : Adafruit_GFX(w, h)
        , WIDTH_(w)
        , HEIGHT_(h)
    {
        const size_t bytes = static_cast<size_t>((w + 3) / 4) * static_cast<size_t>(h);
        buffer_.assign(bytes, 0xFF); // white in 2bpp
        setFullWindow();
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) override
    {
        if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
            return;
        if (mirror_)
            x = width() - x - 1;

        switch (getRotation()) {
        case 1:
            std::swap(x, y);
            x = WIDTH_ - x - 1;
            break;
        case 2:
            x = WIDTH_ - x - 1;
            y = HEIGHT_ - y - 1;
            break;
        case 3:
            std::swap(x, y);
            y = HEIGHT_ - y - 1;
            break;
        default:
            break;
        }

        x -= pw_x_;
        y -= pw_y_;
        if ((x < 0) || (x >= static_cast<int16_t>(pw_w_)) ||
            (y < 0) || (y >= static_cast<int16_t>(pw_h_)))
            return;

        const uint16_t i = static_cast<uint16_t>(x / 4 + y * (pw_w_ / 4));
        if (i >= buffer_.size())
            return;

        buffer_[i] = static_cast<uint8_t>(buffer_[i] & (0xFF ^ (3 << 2 * (3 - x % 4))));
        if (color != 0) {
            uint8_t brb = 0x03;
            if (color != GxEPD_WHITE) {
                const uint32_t brightness =
                    uint32_t(color & 0xF800) +
                    uint32_t((color & 0x07E0) << 5) +
                    uint32_t((color & 0x001F) << 11);
                brb = uint8_t((brightness - 1) / 0xC000ul);
            }
            buffer_[i] = static_cast<uint8_t>(buffer_[i] | (brb << 2 * (3 - x % 4)));
        }
    }

    void drawGreyPixel(int16_t x, int16_t y, uint8_t grey)
    {
        if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
            return;
        if (mirror_)
            x = width() - x - 1;

        switch (getRotation()) {
        case 1:
            std::swap(x, y);
            x = WIDTH_ - x - 1;
            break;
        case 2:
            x = WIDTH_ - x - 1;
            y = HEIGHT_ - y - 1;
            break;
        case 3:
            std::swap(x, y);
            y = HEIGHT_ - y - 1;
            break;
        default:
            break;
        }

        x -= pw_x_;
        y -= pw_y_;
        if ((x < 0) || (x >= static_cast<int16_t>(pw_w_)) ||
            (y < 0) || (y >= static_cast<int16_t>(pw_h_)))
            return;

        const uint16_t i = static_cast<uint16_t>(x / 4 + y * (pw_w_ / 4));
        if (i >= buffer_.size())
            return;

        buffer_[i] = static_cast<uint8_t>(buffer_[i] & (0xFF ^ (3 << 2 * (3 - x % 4))));
        buffer_[i] = static_cast<uint8_t>(buffer_[i] | ((grey >> 6) << 2 * (3 - x % 4)));
    }

    void fillScreen(uint16_t color) override
    {
        const uint32_t brightness =
            uint32_t(color & 0xF800) +
            uint32_t((color & 0x07E0) << 5) +
            uint32_t((color & 0x001F) << 11);
        const uint8_t brb = uint8_t((brightness - 1) / 0xC000ul);
        const uint8_t data = static_cast<uint8_t>(brb * 0b01010101);
        std::fill(buffer_.begin(), buffer_.end(), data);
    }

    void setFullWindow()
    {
        pw_x_ = 0;
        pw_y_ = 0;
        pw_w_ = static_cast<uint16_t>(WIDTH_);
        pw_h_ = static_cast<uint16_t>(HEIGHT_);
    }

    void writeImage_4G(const uint8_t* bitmap, uint8_t bpp,
                       int16_t x, int16_t y, int16_t w, int16_t h,
                       bool invert = false, bool mirror_y = false, bool /*pgm*/ = false)
    {
        if (!bitmap)
            return;

        const uint16_t ppb = (bpp == 2 ? 4 : (bpp == 4 ? 2 : (bpp == 8 ? 1 : 0)));
        if (ppb == 0)
            return;

        const int16_t wb = (w + static_cast<int16_t>(ppb) - 1) / static_cast<int16_t>(ppb);

        for (int16_t row = 0; row < h; ++row) {
            for (int16_t col = 0; col < w; ++col) {
                const int16_t y_pos = mirror_y ? (h - 1 - row) : row;
                const uint32_t byte_idx = static_cast<uint32_t>(col / ppb) + static_cast<uint32_t>(y_pos) * static_cast<uint32_t>(wb);
                uint8_t byte_val = bitmap[byte_idx];
                if (invert)
                    byte_val = static_cast<uint8_t>(~byte_val);

                const uint8_t shift_bits = static_cast<uint8_t>((ppb - 1 - (col % ppb)) * bpp);
                const uint8_t nibble = static_cast<uint8_t>((byte_val >> shift_bits) & ((1u << bpp) - 1u));

                uint8_t grey = 0;
                if (bpp == 2)
                    grey = static_cast<uint8_t>(nibble << 6);
                else if (bpp == 4)
                    grey = static_cast<uint8_t>(nibble << 4);
                else
                    grey = nibble;

                drawGreyPixel(x + col, y + row, grey);
            }
        }
    }

    void display(bool /*partial*/ = false) {}
    void refresh(bool /*partial*/ = false) {}
    void powerOff() {}
    void hibernate() {}
    void init(uint32_t = 0) {}
    void update() {}

    void firstPage()
    {
        fillScreen(GxEPD_WHITE);
    }
    bool nextPage()
    {
         return false;
    }
    bool mirror(bool m)
    {
        const bool prev = mirror_;
        mirror_ = m;
        return prev;
    }

    const uint8_t* getBuffer() const { return buffer_.data(); }
    size_t getBufferSize() const { return buffer_.size(); }

    QImage toImage() const
    {
        QImage img(WIDTH_, HEIGHT_, QImage::Format_Grayscale8);
        static const int levels[4] = {0, 85, 170, 255};
        const int byteWidth = (WIDTH_ + 3) / 4;

        for (int y = 0; y < HEIGHT_; ++y) {
            uint8_t* line = img.scanLine(y);
            for (int x = 0; x < WIDTH_; ++x) {
                const int idx = y * byteWidth + x / 4;
                const int shift = (3 - (x % 4)) * 2;
                const int level = (buffer_[static_cast<size_t>(idx)] >> shift) & 0x03;
                line[x] = static_cast<uint8_t>(levels[level]);
            }
        }
        return img;
    }

    QByteArray to2bpp() const
    {
        return QByteArray(reinterpret_cast<const char*>(buffer_.data()),
                          static_cast<int>(buffer_.size()));
    }

private:
    int16_t WIDTH_;
    int16_t HEIGHT_;
    std::vector<uint8_t> buffer_;
    bool mirror_ = false;
    uint16_t pw_x_ = 0, pw_y_ = 0, pw_w_ = 0, pw_h_ = 0;
};