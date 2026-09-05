//
// Created by miocasa on 8/28/26.
//

#include "TextBox.h"

TextBox::TextBox(const char* str, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color,
                 uint16_t bg_color, uint8_t font_size, Fonts font,
                 VAligns valign, HAligns halign)
	: Widget(x, y, w, h, color, bg_color, WidgetTypes::TEXT_BOX),
	  _size(0),
	  _buffer{},
	  _font_size(font_size),
	  _font_id(font),
	  _valign(valign),
	  _halign(halign),
	  _font(getFontById(_font_id))
{
	_buffer[0] = '\0';
	this->str(str);
}

TextBox::TextBox(const char* str, widget_t* header, text_box_t* data)
	: Widget(header->_x, header->_y, header->_w, header->_h, header->_color, header->_bg_color, WidgetTypes::TEXT_BOX),
	  _size(data->_size),
	  _buffer{},
	  _font_size(data->_font_size),
	  _font_id(data->_font_id),
	  _valign(data->_valign),
	  _halign(data->_halign),
	  _font(getFontById(data->_font_id))
{
	_buffer[0] = '\0';
	this->str(str);
}

const char* TextBox::str() { return _buffer; }
uint8_t TextBox::font_size() { return _font_size; }
size_t TextBox::size() { return _size; }
Fonts TextBox::font() { return _font_id; }
VAligns TextBox::valign() { return _valign; }
HAligns TextBox::halign() { return _halign; }

void TextBox::str(const char* str)
{
	if (!str)
	{
		_buffer[0] = '\0';
		_size = 0;
		return;
	}
	strncpy(_buffer, str, TEXT_BUFFER_SIZE - 1);
	_buffer[TEXT_BUFFER_SIZE - 1] = '\0';
	_size = strlen(_buffer);
}

void TextBox::font_size(uint8_t font_size) { _font_size = font_size; }
void TextBox::font(Fonts font) { _font_id = font; }
void TextBox::valign(VAligns valign) { _valign = valign; }
void TextBox::halign(HAligns halign) { _halign = halign; }

void TextBox::draw(DisplayManager* display)
{
	// TODO: Char by char output
	// TODO partial screen update for one bit
	display->setFont(_font);
	display->setTextColor(get_color());
	display->setTextSize(_font_size);
	int16_t cy, cx;

	int16_t x0, y0;
	uint16_t w, h;
	display->getTextBounds(_buffer, this->get_x(), this->get_y(), &x0, &y0, &w, &h);
	const int16_t ascent = get_y() - y0;

	switch (_valign)
	{
	default:
	case VAligns::Top:
		cy = this->get_y() + ascent;
		break;
	case VAligns::Center:
		cy = this->get_y() + this->get_height() / 2 + ascent / 2;
		break;
	case VAligns::Bottom:
		cy = this->get_y() + this->get_height() + ascent - h;
		break;
	}

	switch (_halign)
	{
	default:
	case HAligns::Left:
		cx = this->get_x();
		break;
	case HAligns::Center:
		cx = this->get_x() + (this->get_width() - w) / 2;
		break;
	case HAligns::Right:
		cx = this->get_x() + this->get_width() - w;
		break;
	}

	display->setCursor(cx, cy);
	display->print(_buffer);
	// display->write(_buffer);
}

void TextBox::partialDraw(DisplayManager* display)
{
}

void TextBox::tick()
{
}
