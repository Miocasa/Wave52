//
// Created by miocasa on 8/28/26.
//

#ifndef EINK_WIDGET_H
#define EINK_WIDGET_H
#include <Arduino.h>
#include <display_detection.h>

#include "widget_t.h"

class DisplayManager;


/*
struct widget_cfg
{
	uint8_t _type;
	int16_t _x, _y;
	uint16_t get_w, get_h;
	uint16_t set_color;
	uint16_t set_bg_color;
};
*/

class Widget // base class for widgets
{
public:
	Widget(int16_t x, int16_t y, uint16_t w, uint16_t h,
	       uint16_t color = GxEPD_BLACK,
	       uint16_t bg_color = GxEPD_WHITE, WidgetTypes type = WidgetTypes::NONE);
	virtual ~Widget() = default;

	// Getters
	WidgetTypes get_type() const; // get set_type
	int16_t get_x() const; // get set_x
	int16_t get_y() const; // get set_y
	uint16_t get_width() const; // get set_width
	uint16_t get_height() const; // get set_height
	uint16_t get_color() const; // get set_color
	uint16_t get_bg_color() const; // get set_bg_color

	// Setters

	/**********************************************************************/
	/**
	 * @brief  Set widget type, needed for safe manager
	 * @param  type		enum(uint8_t) value from widget_types
	 */
	void set_type(WidgetTypes type);

	/**********************************************************************/
	/**
	 * @brief  Set widget x coordinates
	 * @param  x in pixels
	 */
	void set_x(int16_t x);
	/**********************************************************************/
	/**
	 * @brief  Set widget y coordinates
	 * @param  y in pixels
	 */
	void set_y(int16_t y);

	/**********************************************************************/
	/**
	 * @brief  Set widget width
	 * @param  width in pixels
	 */
	void set_width(uint16_t width);

	/**********************************************************************/
	/**
	 * @brief  Set widget height
	 * @param  height in pixels
	 */
	void set_height(uint16_t height);

	/**********************************************************************/
	/**
	 * @brief  Set widget main color
	 * @param  color GxEDP color
	 */
	void set_color(uint16_t color);
	/**********************************************************************/
	/**
	 * @brief  Set widget background color
	 * @param  bg_color GxEDP color
	 */
	void set_bg_color(uint16_t bg_color);


	/**********************************************************************/
	/**
	 * @brief  Draw full widget body
	 * @param  display	DisplayManager ptr to self draw widget content to display buffer
	 */
	virtual void draw(DisplayManager* display) = 0;

	/**********************************************************************/
	/**
	 * @brief  Partial update display work, only with widget where it implemented, works only with one bit background
	 * @param  display	DisplayManager ptr to self draw widget content to display buffer
	 */
	virtual void partialDraw(DisplayManager* display) = 0; // TODO partial screen update for one bit

	/**********************************************************************/
	/**
	 * @brief  Update widget data, like time, battery and sensor values.
	 */
	virtual void tick() = 0;

private:
	WidgetTypes _type;
	int16_t _x, _y;
	uint16_t _w, _h;
	uint16_t _color;
	uint16_t _bg_color;
};


#endif //EINK_WIDGET_H
