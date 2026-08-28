//
// Created by miocasa on 8/28/26.
//

#include "Widgets/Widget.h"

Widget::Widget(int16_t x, int16_t y, uint16_t w, uint16_t h)
	: _x(x), _y(y), _w(w), _h(h)
{
}

int16_t Widget::getX() const { return _x; }
int16_t Widget::getY() const { return _y; }
uint16_t Widget::getWidth() const { return _w; }
uint16_t Widget::getHeight() const { return _h; }

// void Widget::draw(DisplayManager * display)
// {
// }
