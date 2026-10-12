#ifndef EINK_FONTMANAGER_H
#define EINK_FONTMANAGER_H

#include <Arduino.h>
#include <Fonts/F77_Minecraft9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/Monocraft9pt7b.h>
#include <Fonts/Org_01.h>
#include <Fonts/Picopixel.h>
#include <Fonts/Tiny3x3a2pt7b.h>
#include <Fonts/TomThumb.h>

#include "Adafruit_GFX.h"

namespace FontMgr {
  enum class Id : uint8_t {
    Default = 0,
    FreeMono9pt7b,
    FreeSans9pt7b,
    FreeSerif9pt7b,
    Org_01,
    Picopixel,
    Tiny3x3a2pt7b,
    TomThumb,
    Monocraft9pt7b,
    F77_Minecraft9pt7b,
    Count

  };

  struct Entry {
    Id id;
    const char *name;
    const GFXfont *font;
  };

  constexpr Entry kFonts[] = {
      {Id::Default, "Default", nullptr},

      {Id::FreeMono9pt7b, "FreeMono 9pt", &FreeMono9pt7b},
      {Id::FreeSans9pt7b, "FreeSans 9pt", &FreeSans9pt7b},
      {Id::FreeSerif9pt7b, "FreeSerif 9pt", &FreeSerif9pt7b},

      {Id::Org_01, "Org 01", &Org_01},
      {Id::Picopixel, "Picopixel", &Picopixel},
      {Id::Tiny3x3a2pt7b, "Tiny 3x3", &Tiny3x3a2pt7b},
      {Id::TomThumb, "Tom Thumb", &TomThumb},
      {Id::Monocraft9pt7b, "Monocraft 9pt", &Monocraft9pt7b},
      {Id::F77_Minecraft9pt7b, "Minecraft 9pt", &F77_Minecraft9pt7b},
  };

  constexpr uint8_t count() {
    return static_cast<uint8_t>(Id::Count);
  }

  inline const GFXfont *get(Id id) {
    const auto i = static_cast<uint8_t>(id);
    if (i >= count())
      return nullptr;
    return kFonts[i].font;
  }

  inline const char *name(Id id) {
    const auto i = static_cast<uint8_t>(id);
    if (i >= count())
      return "Unknown";
    return kFonts[i].name;
  }

  inline bool valid(Id id) {
    return static_cast<uint8_t>(id) < count();
  }

  inline Id fromUint(uint8_t v) {
    return valid(static_cast<Id>(v)) ? static_cast<Id>(v) : Id::Default;
  }
} // namespace FontMgr

// old implementation capability, temporally TODO will be removed and refactored in future (when I
// end main functional)
using Fonts = FontMgr::Id;
inline const GFXfont *getFontById(Fonts id) {
  return FontMgr::get(id);
}

#endif
