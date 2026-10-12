#pragma once
#ifndef EINK_EXTERNS_H
#define EINK_EXTERNS_H

#include "ExternalRTC.h"
#include "MButton.h"
#include "PowerManager.h"
#include "Settings/SaveManager.h"
#include "Widgets/WidgetManager.h"
// #include "BLE/BLEManager.h"

extern MButton button;
extern DisplayManager display;
extern ExternalRTC externalRTC;
extern WidgetManager widgetManager;
extern SaveManager saveManager;
extern settings_t settings;
// extern BLEManager bleManager;

#endif // EINK_EXTERNS_H
