//
// Created by miocasa on 8/28/26.
//

#include "TextBox.h"


TextBox::TextBox(const char* str, int16_t x, int16_t y, uint16_t w, uint16_t h, text_box_cfg config)
	: Widget(x, y, w, h), _text(str), _config(config)
{
}

void TextBox::draw(DisplayManager* display)
{
	display->setFont(_config.font);
	display->setTextColor(_config.color);
	display->setTextSize(_config.size);
	int16_t y, x;

	int16_t x0, y0;
	uint16_t w, h;
	display->getTextBounds(_text, this->getX(), this->getY(), &x0, &y0, &w, &h);
	const int16_t ascent = getY() - y0;

	const int16_t descent = h - ascent;

	switch (_config.valign)
	{
	default:
	case VAligns::Top:
		y = this->getY() + ascent;
		break;
	case VAligns::Center:
		y = this->getY() + this->getHeight() / 2 + ascent / 2;
		break;
	case VAligns::Bottom:
		y = this->getY() + this->getHeight() + ascent - h;
		break;
	}

	switch (_config.halign)
	{
	default:
	case HAligns::Left:
		x = this->getX();
		break;
	case HAligns::Center:
		x = this->getX() + (this->getWidth() - w) / 2;
		break;
	case HAligns::Right:
		x = this->getX() + this->getWidth() - w;
		break;
	}
	Serial.printf("  cursor_y=%d (h - ascent)=%d  display_h=%d\n", y, h - ascent, this->getHeight());
	Serial.printf("  text top=%d  text height=%d text bot=%d\n", y - ascent, h, y - ascent + h);

	display->setCursor(x, y);
	display->print(_text);
}
