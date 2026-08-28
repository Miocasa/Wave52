//
// Created by miocasa on 8/28/26.
//

#ifndef EINK_WIDGETS_H
#define EINK_WIDGETS_H
#include <Arduino.h>
#include <DisplayManager.h>
// #include "display_detection.h"

// enum Constrains : uint8_t
// {
// 	Left = 0,
// 	Right = 1,
// 	Top = 2,
// 	Bottom = 3,
// 	Center = 4,
// };
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


class Widget // base class for widgets
{
public:
	Widget(int16_t x, int16_t y, uint16_t w, uint16_t h);
	virtual ~Widget() = default;

	int16_t getX() const;
	int16_t getY() const;
	uint16_t getWidth() const;
	uint16_t getHeight() const;

	virtual void draw(DisplayManager* display) = 0;

private:
	int16_t _x;
	int16_t _y;
	uint16_t _w;
	uint16_t _h;
};


#endif //EINK_WIDGETS_H
