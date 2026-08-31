//
// Created by miocasa on 8/28/26.
//

#include "Widgets/Widget.h"

Widget::Widget(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color, uint16_t bg_color, uint8_t type)
	: _type(type), _x(x), _y(y), _w(w), _h(h), _color(color), _bg_color(bg_color)
{
}

uint8_t Widget::type() const { return _type; }
int16_t Widget::x() const { return _x; }
int16_t Widget::y() const { return _y; }
uint16_t Widget::width() const { return _w; }
uint16_t Widget::height() const { return _h; }
uint16_t Widget::color() const { return _color; }
uint16_t Widget::bg_color() const { return _bg_color; }

void Widget::type(uint8_t type) { _type = type; }
void Widget::x(int16_t x) { _x = x; }
void Widget::y(int16_t y) { _y = y; }
void Widget::width(uint16_t width) { _w = width; }
void Widget::height(uint16_t height) { _h = height; }
void Widget::color(uint16_t color) { _color = color; }
void Widget::bg_color(uint16_t bg_color) { _bg_color = bg_color; }


// void Widget::draw(DisplayManager * display)
// {
// }
