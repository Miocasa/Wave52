//
// Created by miocasa on 8/25/26.
//

#include "DisplayManager.h"
// #include "ExternalRTC.get_h"
#include <memory>

#include "Externs.h"
#include "Widgets/TextBox.h"
#include "Widgets/Widget.h"
#include "default_bitmap_2bit.h"

DisplayManager::DisplayManager(int16_t cs, int16_t dc, int16_t rst, int16_t busy)
    : DisplayEpdHolder(cs, dc, rst, busy), _displayBase(_epd) {}

void DisplayManager::update() {
  widgetManager.draw();
  // DateTime now = externalRTC.get_time();
  // char buf[30];
  // sprintf(buf, "Time %02d:%02d:%02d", now.hour(), now.minute(), now.second());
}

/**
        @brief Expansion of original library method to write into library buffer instead of direct
   write to display buffer.\n Author: Jean-Marc Zingg\n Library:
   https://github.com/ZinggJM/GxEPD2_4G

*/
void DisplayManager::writeImage_4G(const uint8_t* bitmap, uint8_t bpp, int16_t x, int16_t y,
                                   int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm) {
  uint16_t ppb = (bpp == 2 ? 4 : (bpp == 4 ? 2 : (bpp == 8 ? 1 : 0)));
  // uint8_t mask = (bpp == 2 ? 0xC0 : (bpp == 4 ? 0xF0 : 0xFF));
  // uint8_t grey1 = (bpp == 2 ? 0x80 : 0xA0); // grey limit for 4bpp

  if (ppb == 0)
    return;

  int16_t wb = (w + ppb - 1) / ppb; // width bytes of bitmap

  for (int16_t row = 0; row < h; row++) {
    for (int16_t col = 0; col < w; col++) {
      int16_t y_pos = mirror_y ? (h - 1 - row) : row;
      uint32_t byte_idx = (col / ppb) + y_pos * wb;
      uint8_t byte_val;

      if (pgm) {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
        byte_val = pgm_read_byte(&bitmap[byte_idx]);
#else
        byte_val = bitmap[byte_idx];
#endif
      } else {
        byte_val = bitmap[byte_idx];
      }

      if (invert)
        byte_val = ~byte_val;

      uint8_t shift_bits = (ppb - 1 - (col % ppb)) * bpp;
      uint8_t nibble = (byte_val >> shift_bits) & ((1 << bpp) - 1);

      uint8_t grey = 0;

      if (bpp == 2) {
        grey = nibble << 6; // nibble * 64
      } else if (bpp == 4) {
        grey = nibble << 4; // nibble * 16
      } else if (bpp == 8) {
        grey = nibble;
      }

      this->drawGreyPixel(x + col, y + row, grey);
    }
  }
}

DisplayManager_BW::DisplayManager_BW(int16_t cs, int16_t dc, int16_t rst, int16_t busy)
    : DisplayEpdHolder(cs, dc, rst, busy), GxEPD2_4G_BW_R(_epd) {}

void DisplayManager_BW::update() {
  widgetManager.draw();
  // DateTime now = externalRTC.get_time();
  // char buf[30];
  // sprintf(buf, "Time %02d:%02d:%02d", now.hour(), now.minute(), now.second());
}

/**
        @brief Expansion of original library method to write into library buffer instead of direct
   write to display buffer.\n Author: Jean-Marc Zingg\n Library:
   https://github.com/ZinggJM/GxEPD2_4G

*/
void DisplayManager_BW::writeImage_4G(const uint8_t* bitmap, uint8_t bpp, int16_t x, int16_t y,
                                      int16_t w, int16_t h, bool invert, bool mirror_y, bool pgm) {
  uint16_t ppb = (bpp == 2 ? 4 : (bpp == 4 ? 2 : (bpp == 8 ? 1 : 0)));
  // uint8_t mask = (bpp == 2 ? 0xC0 : (bpp == 4 ? 0xF0 : 0xFF));
  // uint8_t grey1 = (bpp == 2 ? 0x80 : 0xA0); // grey limit for 4bpp

  if (ppb == 0)
    return;

  int16_t wb = (w + ppb - 1) / ppb; // width bytes of bitmap

  for (int16_t row = 0; row < h; row++) {
    for (int16_t col = 0; col < w; col++) {
      int16_t y_pos = mirror_y ? (h - 1 - row) : row;
      uint32_t byte_idx = (col / ppb) + y_pos * wb;
      uint8_t byte_val;

      if (pgm) {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
        byte_val = pgm_read_byte(&bitmap[byte_idx]);
#else
        byte_val = bitmap[byte_idx];
#endif
      } else {
        byte_val = bitmap[byte_idx];
      }

      if (invert)
        byte_val = ~byte_val;

      uint8_t shift_bits = (ppb - 1 - (col % ppb)) * bpp;
      uint8_t nibble = (byte_val >> shift_bits) & ((1 << bpp) - 1);

      uint8_t grey = 0;

      if (bpp == 2) {
        grey = nibble << 6; // nibble * 64
      } else if (bpp == 4) {
        grey = nibble << 4; // nibble * 16
      } else if (bpp == 8) {
        grey = nibble;
      }

      this->drawPixel(x + col, y + row, grey >= 2 ? GxEPD_WHITE : GxEPD_BLACK);
    }
  }
}
