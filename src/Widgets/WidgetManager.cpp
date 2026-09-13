//
// Created by miocasa on 8/31/26.
//

#include "WidgetManager.h"
#include <Externs.h>

#include <utility>

#include "DisplayManager.h"
#include "image_2bpp.h"
#include "TextBox.h"

WidgetManager::WidgetManager(DisplayManager* disp)
	: _dm(disp), _bg_image(0, 0, display.width(), display.height())
{
	_widgets.reserve(16);
}

void WidgetManager::draw()
{
	_dm->setFullWindow();
	this->drawBackground();
	this->drawWidgets();
	_dm->display();
}

void WidgetManager::partDraw()
{
}

void WidgetManager::drawWidgets()
{
	for (auto& widget : _widgets)
	{
		widget->tick();
		widget->draw(_dm);
	}
}

void WidgetManager::drawBackground()
{
	// _dm->setFullWindow();
	// _dm->fillRect(0, 0, _dm->width(), _dm->height(), GxEPD_WHITE);
	_bg_image.draw(&display);
}


void WidgetManager::setBackground(const char* path)
{
}

void WidgetManager::addWidget(std::shared_ptr<Widget> widget)
{
	_widgets.push_back(std::move(widget));
}

void WidgetManager::addWidget(Widget* widget)
{
	_widgets.push_back(std::shared_ptr<Widget>(widget));
}

void WidgetManager::removeWidget(const std::shared_ptr<Widget>& widget)
{
	for (auto it = _widgets.begin(); it != _widgets.end(); ++it)
	{
		if (it->get() == widget.get())
		{
			_widgets.erase(it);
			return;
		}
	}
}

void WidgetManager::removeWidget(Widget* widget)
{
	for (auto it = _widgets.begin(); it != _widgets.end(); ++it)
	{
		if (it->get() == widget)
		{
			_widgets.erase(it);
			return;
		}
	}
}

void WidgetManager::eraseWidgets()
{
	_widgets.clear();
}

bool WidgetManager::saveScreenToLFS(const char* path)
{
	bool ok = saveManager.saveWidgets(path, &_widgets) == RESULTS::NO_ERROR;
	if (!ok) return false;

	ok = saveManager.saveBackground(path, &_bg_image) == RESULTS::NO_ERROR;
	if (!ok) return false;

	strncpy(settings.screen_path, path, strnlen(path, sizeof(settings.screen_path)));
	saveManager.saveConfig();
	return true;
}

bool WidgetManager::loadScreenFromLFS(const char* path)
{
	bool ok = saveManager.loadWidgets(path, &_widgets) == RESULTS::NO_ERROR;
	if (!ok) return false;

	ok = saveManager.loadBackground(path, &_bg_image) == RESULTS::NO_ERROR;
	if (!ok) return false;

	this->draw();
	return true;
}

bool WidgetManager::loadLastScreen()
{
	bool ok = saveManager.loadConfig(&_widgets, &_bg_image) == RESULTS::NO_ERROR;
	if (ok)
		ok = loadScreenFromLFS(settings.screen_path);

	if (!ok)
		saveManager.loadDefaultScreen(&_widgets, &_bg_image);

	return ok;
}

void WidgetManager::loadDefaultScreen()
{
	saveManager.loadDefaultScreen(&_widgets, &_bg_image);
	this->draw();
}

// void WidgetManager::test()
// {
// 	this->eraseWidgets();
// 	this->addWidget(std::make_shared<TextBox>("str1", 40, 10, 200, 100));
// 	this->addWidget(std::make_shared<TextBox>("str2", 140, 10, 200, 100));
// 	this->addWidget(std::make_shared<TextBox>("str3", 40, 100, 200, 100));
// 	this->_bg_image.set_bitmap(my_2bpp_bitmap, 0, 0, 400, 300, 2);
//
// 	this->draw();
// 	// _dm->display();
// }
