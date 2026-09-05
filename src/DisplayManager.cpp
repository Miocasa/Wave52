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

// class SaveManager;
// SaveManager saveManager;

DisplayManager::DisplayManager(int16_t cs, int16_t dc, int16_t rst, int16_t busy)
	: DisplayEpdHolder(cs, dc, rst, busy),
	  _displayBase<_display, _display::HEIGHT>(_epd)
{
}

/*void DisplayManager::drawWidgets() // deprecated
{
	// widgetManager.drawBackground();
	// widgetManager.drawWidgets();
	//
	//
	// return;

	update();
	return;

	DateTime now = externalRTC.get_time();
	char buf[30];
	sprintf(buf, "Time %02d:%02d:%02d", now.hour(), now.minute(), now.second());

	int16_t w = set_width();
	int16_t h = set_height();
	int16_t set_x = 0;
	int16_t set_y = 0;

	this->fillRect(0, 0, w, h, GxEPD_WHITE);
	std::unique_ptr<Widget> text = std::unique_ptr<TextBox>(
		new TextBox(buf, set_x, set_y, w, h, GxEPD_BLACK,GxEPD_WHITE, 2,
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
} //! deprecated*/

void DisplayManager::update()
{
	widgetManager.draw();
	// DateTime now = externalRTC.get_time();
	// char buf[30];
	// sprintf(buf, "Time %02d:%02d:%02d", now.hour(), now.minute(), now.second());
}
