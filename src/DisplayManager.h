//
// Created by miocasa on 8/25/26.
//

#ifndef EINK_DISPLAYMANAGER_H
#define EINK_DISPLAYMANAGER_H
#include <config.h>
#include <memory>
#include <vector>

#include "display_detection.h"
#include "Widgets/Widget.h"
// struct


class DisplayEpdHolder
{
public:
	virtual ~DisplayEpdHolder() = default;

protected:
	DisplayEpdHolder(int16_t cs, int16_t dc, int16_t rst, int16_t busy)
		: _epd(cs, dc, rst, busy)
	{
	}

	_display _epd;
};


class DisplayManager : private DisplayEpdHolder, public _displayBase<_display, _display::HEIGHT>
{
public:
	DisplayManager(int16_t cs, int16_t dc, int16_t rst, int16_t busy);
	~DisplayManager() override = default;

	void drawWidgets();
	void update();
private:
	std::vector<std::unique_ptr<Widget>> _widgets;

protected:
	void testGrid();
};


#endif //EINK_DISPLAYMANAGER_H
