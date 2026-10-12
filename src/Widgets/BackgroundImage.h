//
// Created by miocasa on 8/31/26.
//

#ifndef EINK_BACKGROUNDIMAGE_H
#define EINK_BACKGROUNDIMAGE_H
#include <cstdint>

#include "DisplayManager.h"
#include "Widget.h"

class BackgroundImage : public Widget {
 public:
  BackgroundImage(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t bpp = 2);
  BackgroundImage(const uint8_t *bitmap, widget_t header, bg_image_t bg_image);
  ~BackgroundImage() override;

  // Getters
  uint8_t get_bpp();
  const uint8_t *get_bitmap();
  size_t get_size();

  // Setters
  void set_size(size_t size);

  void set_bpp(uint8_t bpp);

  void set_bitmap(const uint8_t *bitmap, int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t bpp,
                  uint16_t color = GxEPD_BLACK);

  void draw(DisplayManager *display) override;
  void partialDraw(DisplayManager *display) override;
  void clear(uint8_t color); // 0xff 0x00
  void tick() override;
  void set_bitmap_part(const uint8_t *src, uint32_t start, uint16_t len);

 private:
  uint8_t _bpp; // bits per pixel, 1 bit or 2 bit bitmap set_color depth
  uint8_t *_bitmap;
  size_t bitmap_size;
};
#endif // EINK_BACKGROUNDIMAGE_H
