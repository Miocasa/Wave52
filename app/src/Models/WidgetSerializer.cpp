//
// Created by miocasa on 9/22/26.
//

#include "WidgetSerializer.h"

#include <cstdint>
#include <cstring>

#include "../Protocol.h"

namespace {

  void putU16(uint8_t* p, uint16_t v) {
    p[0] = static_cast<uint8_t>(v & 0xFF);
    p[1] = static_cast<uint8_t>(v >> 8);
  }

  void putU32(uint8_t* p, uint32_t v) {
    p[0] = static_cast<uint8_t>(v & 0xFF);
    p[1] = static_cast<uint8_t>((v >> 8) & 0xFF);
    p[2] = static_cast<uint8_t>((v >> 16) & 0xFF);
    p[3] = static_cast<uint8_t>((v >> 24) & 0xFF);
  }

  void putF32(uint8_t* p, float v) {
    uint32_t bits = 0;
    std::memcpy(&bits, &v, sizeof(bits));
    putU32(p, bits);
  }

} // namespace

QByteArray WidgetSerializer::textBox(const TextBoxSpec& spec) {
  const QByteArray text = spec.text.toUtf8();
  if (text.size() > Layout::kMaxTextBytes) {
    return {};
  }

  QByteArray out(Layout::kWidgetSize + Layout::kTextBoxSize, 0);
  auto* w = reinterpret_cast<uint8_t*>(out.data());

  putF32(w + Layout::kWidgetOffVersion, Layout::kWidgetVersion);
  w[Layout::kWidgetOffType] = static_cast<uint8_t>(Proto::WidgetTypes::kTypeTextBox);
  putU16(w + Layout::kWidgetOffX, static_cast<uint16_t>(static_cast<int16_t>(spec.x)));
  putU16(w + Layout::kWidgetOffY, static_cast<uint16_t>(static_cast<int16_t>(spec.y)));
  putU16(w + Layout::kWidgetOffW, static_cast<uint16_t>(spec.w));
  putU16(w + Layout::kWidgetOffH, static_cast<uint16_t>(spec.h));
  putU16(w + Layout::kWidgetOffColor, static_cast<uint16_t>(spec.color));
  putU16(w + Layout::kWidgetOffBgColor, static_cast<uint16_t>(spec.bgColor));

  uint8_t* t = w + Layout::kWidgetSize;
  t[Layout::kTextBoxOffFontSize] = static_cast<uint8_t>(spec.fontSize);
  t[Layout::kTextBoxOffFontId] = static_cast<uint8_t>(spec.fontId);
  t[Layout::kTextBoxOffValign] = static_cast<uint8_t>(spec.valign);
  t[Layout::kTextBoxOffHalign] = static_cast<uint8_t>(spec.halign);
  putU32(t + Layout::kTextBoxOffTextSize, static_cast<uint32_t>(text.size()));

  out.append(text);
  return out;
}
