//
// Created by miocasa on 8/30/26.
//

#ifndef EINK_FONTMANAGER_H
#define EINK_FONTMANAGER_H

#include <cstdint>

#include "Adafruit_GFX.h"
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/Org_01.h>
#include <Fonts/Picopixel.h>
#include <Fonts/Tiny3x3a2pt7b.h>
#include <Fonts/TomThumb.h>

#include <Fonts/Monocraft9pt7b.h>
#include <Fonts/F77_Minecraft9pt7b.h>

enum class Fonts : uint8_t
{
	Default = 0,
	FreeMono9pt7b_ID,
	FreeSans9pt7b_ID,
	FreeSerif9pt7b_ID,
	Org_01_ID,
	Picopixel_ID,
	Tiny3x3a2pt7b_ID,
	TomThumb_ID,

	// Self added fonts, in src/Fonts dir
	Monocraft9pt7b_ID,
	F77_Minecraft9pt7b_ID,
};

inline const GFXfont* getFontById(const Fonts id)
{
	switch (id)
	{
	default:
	case Fonts::Default:
		return nullptr;
	case Fonts::FreeMono9pt7b_ID:
		return &FreeMono9pt7b;
	case Fonts::FreeSans9pt7b_ID:
		return &FreeSans9pt7b;
	case Fonts::FreeSerif9pt7b_ID:
		return &FreeSerif9pt7b;
	case Fonts::Org_01_ID:
		return &Org_01;
	case Fonts::Picopixel_ID:
		return &Picopixel;
	case Fonts::Tiny3x3a2pt7b_ID:
		return &Tiny3x3a2pt7b;
	case Fonts::TomThumb_ID:
		return &TomThumb;

	// Self added fonts
	case Fonts::Monocraft9pt7b_ID:
		return &Monocraft9pt7b;
	case Fonts::F77_Minecraft9pt7b_ID:
		return &F77_Minecraft9pt7b;
	}
	return nullptr;
}

#endif //EINK_FONTMANAGER_H
