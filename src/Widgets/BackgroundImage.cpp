//
// Created by miocasa on 9/6/26.
//

#include "BackgroundImage.h"

BackgroundImage::BackgroundImage(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t bpp)
	: Widget(x, y, w, h, GxEPD_BLACK, GxEPD_WHITE, WidgetTypes::BG_IMAGE), _bpp(bpp), _bitmap(nullptr), bitmap_size(0)
{
	bitmap_size = static_cast<size_t>(w * h / (8 / bpp));

	_bitmap = new uint8_t[bitmap_size];
	memset((void*)_bitmap, 0, bitmap_size);
}

BackgroundImage::~BackgroundImage()
{
	if (_bitmap)
	{
		delete[] _bitmap;
		_bitmap = nullptr;
	}
}

uint8_t BackgroundImage::get_bpp()
{
	return _bpp;
}

size_t BackgroundImage::get_size()
{
	return bitmap_size;
}

void BackgroundImage::set_size(size_t size)
{
	bitmap_size = size;
}

void BackgroundImage::set_bpp(uint8_t bpp)
{
	_bpp = bpp;
}

const uint8_t* BackgroundImage::get_bitmap()
{
	return _bitmap;
}

void BackgroundImage::set_bitmap(const uint8_t* bitmap, int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t bpp,
                                 uint16_t color)
{
	set_x(x);
	set_y(y);
	set_width(w);
	set_height(h);
	set_color(color);
	_bpp = bpp;

	if (color == GxEPD_BLACK || color == GxEPD_DARKGREY)
		set_bg_color(GxEPD_WHITE);
	else
		set_bg_color(GxEPD_BLACK);

	size_t size = get_width() * get_height() / (8 / bpp);
	if (bitmap && bitmap_size >= size)
	{
		memcpy((void*)_bitmap, bitmap, size);
	}
}

void BackgroundImage::draw(DisplayManager* display)
{
	if (!display || !_bitmap) return;
	display->writeImage_4G(_bitmap, get_bpp(), get_x(), get_y(), get_width(), get_height());
}

void BackgroundImage::partialDraw(DisplayManager* display)
{
}

void BackgroundImage::tick()
{
}

