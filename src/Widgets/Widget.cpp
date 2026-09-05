//
// Created by miocasa on 8/28/26.
//

#include "Widgets/Widget.h"

Widget::Widget(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color, uint16_t bg_color, WidgetTypes type)
	: _type(type), _x(x), _y(y), _w(w), _h(h), _color(color), _bg_color(bg_color)
{
}

WidgetTypes Widget::get_type() const { return _type; }
int16_t Widget::get_x() const { return _x; }
int16_t Widget::get_y() const { return _y; }
uint16_t Widget::get_width() const { return _w; }
uint16_t Widget::get_height() const { return _h; }
uint16_t Widget::get_color() const { return _color; }
uint16_t Widget::get_bg_color() const { return _bg_color; }

void Widget::set_type(WidgetTypes type) { _type = type; }
void Widget::set_x(int16_t x) { _x = x; }
void Widget::set_y(int16_t y) { _y = y; }
void Widget::set_width(uint16_t width) { _w = width; }
void Widget::set_height(uint16_t height) { _h = height; }
void Widget::set_color(uint16_t color) { _color = color; }
void Widget::set_bg_color(uint16_t bg_color) { _bg_color = bg_color; }


// void Widget::draw(DisplayManager * display)
// {
// }
