#pragma once
#ifndef EINK_EXTERNS_H
#define EINK_EXTERNS_H

#include "ExternalRTC.h"
#include "PowerManager.h"
#include "SaveManager/SaveManager.h"
#include "Widgets/WidgetManager.h"

extern DisplayManager display;
extern ExternalRTC externalRTC;
extern NRFPowerManager power;
extern WidgetManager widgetManager;
extern SaveManager saveManager;
#endif //EINK_EXTERNS_H
