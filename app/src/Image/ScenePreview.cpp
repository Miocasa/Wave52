#include "ScenePreview.h"

#include "EinkCanvas.h"
#include "Widgets/TextBox.h"

QImage ScenePreview::render(const QByteArray& background, int bgWidth, int bgHeight,
                            const std::vector<TextBoxSpec>& widgets) {
  EinkCanvas canvas;
  canvas.firstPage();

  if (!background.isEmpty() && bgWidth > 0 && bgHeight > 0) {
    canvas.writeImage_4G(reinterpret_cast<const uint8_t*>(background.constData()), 2, 0, 0,
                         static_cast<int16_t>(bgWidth), static_cast<int16_t>(bgHeight));
  }

  for (const TextBoxSpec& spec: widgets) {
    const QByteArray text = spec.text.toUtf8();
    TextBox box(text.constData(), static_cast<int16_t>(spec.x), static_cast<int16_t>(spec.y),
                static_cast<uint16_t>(spec.w), static_cast<uint16_t>(spec.h), static_cast<uint16_t>(spec.color),
                static_cast<uint16_t>(spec.bgColor), static_cast<uint8_t>(spec.fontSize),
                static_cast<Fonts>(spec.fontId), static_cast<VAligns>(spec.valign), static_cast<HAligns>(spec.halign));
    box.draw(&canvas);
  }

  return canvas.toImage();
}
