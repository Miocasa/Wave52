//
// Created by miocasa on 8/31/26.
//

#ifndef EINK_WIDGETMANAGER_H
#define EINK_WIDGETMANAGER_H
#include <memory>
#include <vector>
#include "Widgets/Widget.h"
#include "BackgroundImage.h"

class DisplayManager;

class WidgetManager
{
public:
	WidgetManager(DisplayManager* disp);

	void draw();
	void partDraw();

	void drawWidgets();
	void drawBackground();

	void setBackground(const char* path);
	void addWidget(std::shared_ptr<Widget> widget);
	void addWidget(Widget* widget);
	void removeWidget(const std::shared_ptr<Widget>& widget);
	void removeWidget(Widget* widget);
	void eraseWidgets();

	bool saveScreenToLFS(const char* path);
	bool loadScreenFromLFS(const char* path);

	bool loadLastScreen();

	void loadDefaultScreen();
	// void test();

private:
	DisplayManager* _dm;

	BackgroundImage _bg_image;
	std::vector<std::shared_ptr<Widget>> _widgets;
};


#endif //EINK_WIDGETMANAGER_H
