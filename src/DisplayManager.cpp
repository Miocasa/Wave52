//
// Created by miocasa on 8/25/26.
//

#include "DisplayManager.h"
// #include "ExternalRTC.h"
#include "Externs.h"
#include "image_2bpp.h"


DisplayManager::DisplayManager(int16_t cs, int16_t dc, int16_t rst, int16_t busy)
	: DisplayEpdHolder(cs, dc, rst, busy),
	  _displayBase<_display, _display::HEIGHT>(_epd)


{
}

void DisplayManager::drawWidgets()
{
}

void DisplayManager::test()
{
	DateTime now = externalRTC.get_time();
	char buf[30];
	sprintf(buf, "Time %02d:%02d:%02d", now.hour(), now.minute(), now.second());

	// this->drawGreyPixmap();

	this->setTextColor(GxEPD_BLACK);
	this->setFont(&FreeSansBold12pt7b);
	this->setTextSize(2);
	uint16_t tbw, tbh;
	int16_t tbx, tby;
	this->getTextBounds(buf, 0, 0, &tbx, &tby, &tbw, &tbh);
	int16_t x = (width() - tbw) / 2 - tbx;
	int16_t y = (height() - tbh) / 2 - tby;

	this->setFullWindow();
	this->firstPage();
	do
	{
		// this->fillRect(0, 0, width(), height(), GxEPD_WHITE);
		this->drawGreyPixmap(my_2bpp_bitmap, 2, 0, 0, 400, 300);

		this->setCursor(x, y);
		this->print(buf);
	}
	while (this->nextPage());

	this->hibernate();
}
