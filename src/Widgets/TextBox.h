//
// Created by miocasa on 8/28/26.
//

#ifndef EINK_TEXTBOX_H
#define EINK_TEXTBOX_H
#include <Adafruit_GFX.h>
#include "DisplayManager.h"
#include "Widget.h"

struct text_box_cfg
{
	uint16_t color = GxEPD_BLACK;
	uint8_t size;
	const GFXfont* font = nullptr;
	VAligns valign;
	HAligns halign;

	text_box_cfg(uint16_t c, uint8_t s, const GFXfont* f,
	             VAligns valign = VAligns::Top, HAligns halign = HAligns::Left)
		: color(c), size(s), font(f), valign(valign), halign(halign)
	{
	}

	text_box_cfg() = default;
};

class TextBox : public Widget
{
public:
	TextBox(const char* str, int16_t x, int16_t y, uint16_t w, uint16_t h, text_box_cfg config);
	~TextBox() override = default;

	void draw(DisplayManager* display) override; // TODO: Char by char output
	// TODO partial screen update for one bit
private:
	const char* _text;
	const text_box_cfg _config;
	// const GFXfont* _font = nullptr;
	// Constrains _constrain;
	// uint16_t _color = GxEPD_BLACK;
};


#endif //EINK_TEXTBOX_H
