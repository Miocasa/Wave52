//
// Created by miocasa on 9/6/26.
//

#include "BackgroundImage.h"

#include "Widgets/widget_t.h"

BackgroundImage::BackgroundImage(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t bpp)
    : Widget(x, y, w, h, GxEPD_BLACK, GxEPD_WHITE, WidgetTypes::kTypeBackground),
      _bpp(bpp),
      _bitmap(nullptr),
      bitmap_size(0) {
  bitmap_size = static_cast<size_t>(w * h / (8 / bpp));

  _bitmap = new uint8_t[bitmap_size];
  memset((void *)_bitmap, 0, bitmap_size);
}

BackgroundImage::BackgroundImage(const uint8_t *bitmap, widget_t header, bg_image_t bg_image)
    : Widget(header._x, header._y, header._w, header._h, header._color, header._bg_color,
             WidgetTypes::kTypeBackground),
      _bpp(bg_image.bpp),
      _bitmap(nullptr),
      bitmap_size(bg_image.size) {
  bitmap_size = static_cast<size_t>(header._w * header._h / (8 / bg_image.bpp));
  _bitmap = new uint8_t[bitmap_size];

  memcpy((void *)_bitmap, bitmap, bitmap_size);
}

BackgroundImage::~BackgroundImage() {
  if (_bitmap) {
    delete[] _bitmap;
    _bitmap = nullptr;
  }
}

uint8_t BackgroundImage::get_bpp() {
  return _bpp;
}

size_t BackgroundImage::get_size() {
  return bitmap_size;
}

void BackgroundImage::set_size(size_t size) {
  bitmap_size = size;
}

void BackgroundImage::set_bpp(uint8_t bpp) {
  _bpp = bpp;
}

const uint8_t *BackgroundImage::get_bitmap() {
  return _bitmap;
}

void BackgroundImage::set_bitmap(const uint8_t *bitmap, int16_t x, int16_t y, uint16_t w,
                                 uint16_t h, uint8_t bpp, uint16_t color) {
  set_x(x);
  set_y(y);
  set_width(w);
  set_height(h);
  set_color(color);
  _bpp = bpp;

  if (color == GxEPD_BLACK || color == GxEPD_DARKGREY)
    set_bg_color(GxEPD_WHITE);
  else
    set_bg_color(GxEPD_BLACK);

  size_t size = get_width() * get_height() / (8 / bpp);
  if (bitmap && bitmap_size >= size) {
    memcpy((void *)_bitmap, bitmap, size);
  }
}

void BackgroundImage::draw(DisplayManager *display) {
  if (!display || !_bitmap)
    return;
  display->writeImage_4G(_bitmap, get_bpp(), get_x(), get_y(), get_width(), get_height());
}

void BackgroundImage::partialDraw(DisplayManager *display) {}

void BackgroundImage::clear(uint8_t color) {
  if (_bitmap && bitmap_size)
    memset(_bitmap, color, bitmap_size);
}

void BackgroundImage::tick() {}

void BackgroundImage::set_bitmap_part(const uint8_t *src, uint32_t start, uint16_t len) {
  if (!src || !_bitmap || len == 0)
    return;

  if (start >= bitmap_size) {
    Serial.printf("set_bitmap_part: start %u >= size %u\n", start, (unsigned)bitmap_size);
    return;
  }

  uint32_t can_copy = bitmap_size - start;
  if (len > can_copy) {
    Serial.printf("set_bitmap_part: clamp len %u → %u (start=%u)\n", len, can_copy, start);
    len = static_cast<uint16_t>(can_copy);
  }

  memcpy(_bitmap + start, src, len);
}
