//
// Created by miocasa on 8/31/26.
//

#ifndef EINK_WIDGETMANAGER_H
#define EINK_WIDGETMANAGER_H
#include <memory>
#include <vector>

#include "BackgroundImage.h"
#include "Widgets/Widget.h"

class DisplayManager;

class WidgetManager {
 public:
  WidgetManager(DisplayManager *disp);

  // Rendering
  void draw();
  void partDraw();
  void drawWidgets();
  void drawBackground();

  // Background and widgets management
  void setBackground(const char *path);
  void addWidget(std::shared_ptr<Widget> widget);
  void addWidget(Widget *widget);
  void removeWidget(const std::shared_ptr<Widget> &widget);
  void removeWidget(Widget *widget);
  void eraseWidgets();

  // Screen functions
  bool saveScreenToLFS(uint8_t slot);
  bool loadScreenFromLFS(uint8_t slot);
  bool loadLastScreen();
  void loadDefaultScreen();

  BackgroundImage *getBackground(); // for direct interaction

 private:
  DisplayManager *_dm;

  BackgroundImage _bg_image;
  std::vector<std::shared_ptr<Widget>> _widgets;
};

#endif // EINK_WIDGETMANAGER_H
