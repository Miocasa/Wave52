//
// Created by miocasa on 8/25/26.
//

#include "DisplayManager.h"
// #include "ExternalRTC.get_h"
#include <memory>

#include "Externs.h"
#include "image_2bpp.h"
#include "Widgets/TextBox.h"
#include "Widgets/Widget.h"
#include <Fonts/FreeSans24pt7b.h>

DisplayManager::DisplayManager(int16_t cs, int16_t dc, int16_t rst, int16_t busy)
	: DisplayEpdHolder(cs, dc, rst, busy),
	  _displayBase<_display, _display::HEIGHT>(_epd)
{
}

void DisplayManager::drawWidgets()
{
	DateTime now = externalRTC.get_time();
	char buf[30];
	sprintf(buf, "Time %02d:%02d:%02d", now.hour(), now.minute(), now.second());

	int16_t w = width();
	int16_t h = height();
	int16_t x = 0;
	int16_t y = 0;

	this->fillRect(0, 0, w, h, GxEPD_WHITE);
	std::unique_ptr<Widget> text = std::unique_ptr<TextBox>(
		new TextBox(buf, x, y, w, h, GxEPD_BLACK,GxEPD_WHITE, 2,
		            Fonts::Monocraft9pt7b_ID, VAligns::Center,
		            HAligns::Center)
	);
	// auto config = (std::unique_ptr<widget_cfg>());

	//! Cast test
	if (auto casted = reinterpret_cast<TextBox*>(text.get())) { Serial.println("Successfully casted"); }
	else { Serial.println("Failed to cast to text_box_cfg"); }

	text->draw(this);
	// delete text; // unique_ptr self free memory

	// this->drawGreyPixmap();

	this->display();
	this->hibernate();
}

void DisplayManager::update()
{
	drawWidgets();
}

void DisplayManager::testGrid()
{
	// const char* buf = "";
	this->fillRect(0, 0, this->width(), this->height(), GxEPD_WHITE);

	this->drawPixel(10, 10, GxEPD_BLACK);


	// int16_t get_w = 200;
	// int16_t get_h = 200;
	// int16_t _x = 20;
	// int16_t _y = 100;
	int16_t w = width();
	int16_t h = height();
	int16_t x = 0;
	int16_t y = 0;
	this->drawRect(x, y, w, h, GxEPD_BLACK);

	VAligns va[] = {VAligns::Top, VAligns::Center, VAligns::Bottom};
	HAligns ha[] = {HAligns::Left, HAligns::Center, HAligns::Right};

	auto font = static_cast<uint8_t>(Fonts::FreeMono9pt7b_ID);
	for (auto& i : va)
		for (auto& j : ha)
		{
			std::unique_ptr<Widget> text = std::unique_ptr<TextBox>(
				new TextBox("str", x, y, w, h, GxEPD_BLACK,GxEPD_WHITE, 3, static_cast<Fonts>(font++), i, j)
			);
			text->draw(this);
		}

	this->display();
}
