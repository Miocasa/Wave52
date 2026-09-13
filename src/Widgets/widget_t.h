//
// Created by miocasa on 9/5/26.
//

#ifndef EINK_WIDGET_TYPES_H
#define EINK_WIDGET_TYPES_H
#include "FontManager.h"
#include "Widget.h"

enum class WidgetTypes : uint8_t
{
	NONE = 0,
	TEXT_BOX = 1,
	BG_IMAGE = 0xFF,
};


enum class HAligns : uint8_t
{
	Left = 0,
	Center = 1,
	Right = 2
};

enum class VAligns : uint8_t
{
	Top = 0,
	Center = 1,
	Bottom = 2,
};


struct widget_t
{
	float _version;
	WidgetTypes _type;
	int16_t _x, _y;
	uint16_t _w, _h;
	uint16_t _color;
	uint16_t _bg_color;
};

struct text_box_t
{
	uint8_t _font_size;
	Fonts _font_id;
	VAligns _valign;
	HAligns _halign;
	size_t _size;

	// casted->str() write separated
};

struct bg_image_t
{
	uint8_t bpp; // bits per pixel, 1 bit or 2 bit bitmap set_color depth
	size_t size;
	// const char* bitmap; // write separated
};

#endif //EINK_WIDGET_TYPES_H
