//
// Created by miocasa on 8/25/26.
//

#ifndef EINK_DISPLAYMANAGER_H
#define EINK_DISPLAYMANAGER_H
#include <config.h>

#include "display_detection.h"


class DisplayManager : GxEPD2_4G_4G<_display, _display::HEIGHT>
{
	GxEPD2_4G_4G
};


#endif //EINK_DISPLAYMANAGER_H
