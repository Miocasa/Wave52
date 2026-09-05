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
	: _dm(disp)
{
	_widgets.reserve(16);

	addWidget(std::make_shared<TextBox>("str1", 40, 10, 200, 100));
	addWidget(std::make_shared<TextBox>("str2", 140, 10, 200, 100));
	addWidget(std::make_shared<TextBox>("str3", 40, 100, 200, 100));
}

void WidgetManager::draw()
{
	this->drawBackground();
	this->drawWidgets();
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
	_dm->display();
}

void WidgetManager::drawBackground()
{
	_dm->fillRect(0, 0, _dm->width(), _dm->height(), GxEPD_WHITE);

	_dm->setFullWindow();
}


void WidgetManager::setBackground(BackgroundImage* background)
{
	this->_bg_image = std::shared_ptr<BackgroundImage>(background);
}

void WidgetManager::setBackground(std::shared_ptr<BackgroundImage> background)
{
	this->_bg_image = std::move(background);
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
	return saveManager.saveWidgets(path, &_widgets) == RESULTS::NO_ERROR;
}

bool WidgetManager::loadScreenFromLFS(const char* path)
{
	bool ok = saveManager.loadWidgets(path, &_widgets) == RESULTS::NO_ERROR;

	if (ok) draw();
	return ok;
}
