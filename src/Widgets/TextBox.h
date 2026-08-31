
// Created by miocasa on 8/28/26.
//

#ifndef EINK_TEXTBOX_H
#define EINK_TEXTBOX_H
#include <DisplayManager.h>
#include "FontManager.h"
#include "Widget.h"
/*
struct text_box_cfg : public widget_cfg
{
	// int16_t _x, _y;
	// uint16_t get_w, get_h;
	// uint8_t _type;
	// uint16_t color;
	// uint16_t bg_color;
	const char* text;
	uint8_t font_size;
	const GFXfont* _font = nullptr;
	VAligns _valign;
	HAligns _halign;
};
*/
#define TEXT_BUFFER_SIZE 128

class TextBox : public Widget
{
public:
	TextBox(const char* str, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color = GxEPD_BLACK,
	        uint16_t bg_color = GxEPD_WHITE, uint8_t font_size = 1, Fonts font = Fonts::Default,
	        VAligns valign = VAligns::Top, HAligns halign = HAligns::Left);
	~TextBox() override = default;

	// Getters
	const char* str();
	uint8_t font_size();
	Fonts font();
	VAligns valign();
	HAligns halign();

	size_t size();

	// Setters
	void str(const char* str);
	void font_size(uint8_t font_size);
	void font(Fonts font);
	void valign(VAligns valign);
	void halign(HAligns halign);

	void draw(DisplayManager* display) override;
	void partialDraw(DisplayManager* display) override;

private:
	size_t _size;
	char _buffer[TEXT_BUFFER_SIZE]; // str
	uint8_t _font_size;
	Fonts _font_id;
	VAligns _valign;
	HAligns _halign;

	const GFXfont* _font = nullptr;
};


#endif //EINK_TEXTBOX_H
