//
// Created by miocasa on 8/28/26.
//

#ifndef EINK_WIDGET_H
#define EINK_WIDGET_H
#include <Arduino.h>
#include <display_detection.h>

class DisplayManager;


enum widget_types : uint8_t
{
	WIDGET_TYPE_NONE = 0,
	WIDGET_TYPE_TEXT_BOX = 1,
};

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

/*
struct widget_cfg
{
	uint8_t _type;
	int16_t _x, _y;
	uint16_t get_w, get_h;
	uint16_t color;
	uint16_t bg_color;

	widget_cfg(uint8_t _type, int16_t _x, int16_t _y, uint16_t get_w, uint16_t get_h, uint16_t color = GxEPD_BLACK,
	           uint16_t bg_color = GxEPD_WHITE)
		: _type(_type), _x(_x), _y(_y), get_w(get_w), get_h(get_h), color(color), bg_color(bg_color)
	{
	}

	~widget_cfg() = default;

	void debugPrint() const
	{
		Serial.printf("{\n  _type=%d\n  _x=%d,_y=%d\n  get_w=%d,get_h=%d\n  color=0x%X,bg_color=0x%X\n}\n",
		              _type, _x, _y, get_w, get_h, color, bg_color);
	}
};
*/

class Widget // base class for widgets
{
public:
	Widget(int16_t x, int16_t y, uint16_t w, uint16_t h,
	       uint16_t color = GxEPD_BLACK,
	       uint16_t bg_color = GxEPD_WHITE, uint8_t type = WIDGET_TYPE_NONE);
	virtual ~Widget() = default;

	// Getters
	uint8_t type() const; // get type
	int16_t x() const; // get get_x
	int16_t y() const; // get get_y
	uint16_t width() const; // get width
	uint16_t height() const; // get height
	uint16_t color() const; // get color
	uint16_t bg_color() const; // get bg_color

	// Setters
	void type(uint8_t type);
	void x(int16_t x);
	void y(int16_t y);
	void width(uint16_t width);
	void height(uint16_t height);
	void color(uint16_t color);
	void bg_color(uint16_t bg_color);


	// virtual void printConfigSerial();
	virtual void draw(DisplayManager* display) = 0;
	virtual void partialDraw(DisplayManager* display) = 0; // TODO partial screen update for one bit

private:
	uint8_t _type;
	int16_t _x, _y;
	uint16_t _w, _h;
	uint16_t _color;
	uint16_t _bg_color;
};


#endif //EINK_WIDGET_H


// struct widget_cfg
// {
// private:
// 	uint8_t _type;
// 	int16_t _x, _y;
// 	uint16_t _w, _h;
// 	uint16_t _color;
// 	uint16_t _bg_color;
//
// public:
// 	widget_cfg(uint8_t _type, int16_t _x, int16_t _y, uint16_t get_w, uint16_t get_h, uint16_t color = GxEPD_BLACK,
// 			   uint16_t bg_color = GxEPD_WHITE)
// 		: _type(_type), _x(_x), _y(_y), _w(get_w), _h(get_h), _color(color), _bg_color(bg_color)
// 	{
// 	}
//
// 	uint8_t _type() const { return _type; }
// 	int16_t _x() const { return _x; }
// 	int16_t _y() const { return _y; }
// 	uint16_t get_w() const { return _w; }
// 	uint16_t get_h() const { return _h; }
// 	uint16_t color() const { return _color; }
// 	uint16_t bg_color() const { return _bg_color; }
// 	void _type(uint8_t _type);
// 	void _x(int16_t _x);
// 	void _y(int16_t _y);
// 	void get_w(uint16_t get_w);
// 	void get_h(uint16_t get_h);
// 	void color(uint16_t color);
// 	void bg_color(uint16_t color);
// };
