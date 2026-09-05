//
// Created by miocasa on 8/31/26.
//

#ifndef EINK_BACKGROUNDIMAGE_H
#define EINK_BACKGROUNDIMAGE_H
#include <cstdint>
#include <InternalFileSystem.h>

#include "Widget.h"


class BackgroundImage : public Widget // todo add functions implementation
{
	BackgroundImage(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t bpp, const char* bitmap)
		: Widget(x, y, w, h, GxEPD_BLACK, GxEPD_WHITE, WidgetTypes::BG_IMAGE), bpp(bpp), bitmap(bitmap)
	{
	}

	const char* get_bitmap();
	void set_bitmap(const char* bitmap, uint16_t color);

	void draw(DisplayManager* display) override;

private:
	uint8_t bpp; // bits per pixel, 1 bit or 2 bit bitmap set_color depth
	const char* bitmap;
};
#endif //EINK_BACKGROUNDIMAGE_H
