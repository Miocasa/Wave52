#pragma once

#include <QByteArray>
#include <QImage>

#include <vector>

#include "Models/WidgetSerializer.h"

namespace ScenePreview {

  QImage render(const QByteArray& background, int bgWidth, int bgHeight, const std::vector<TextBoxSpec>& widgets);

}
