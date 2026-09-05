//
// Created by miocasa on 9/5/26.
//
// todo unit tests

/*
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
				new TextBox("str", x, y, w, h, GxEPD_BLACK, GxEPD_WHITE, 3, static_cast<Fonts>(font++), i, j)
			);
			text->draw(this);
		}

	this->display();
} //! deprecated
*/
