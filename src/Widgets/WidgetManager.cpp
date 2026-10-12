//
// Created by miocasa on 8/31/26.
//

#include "WidgetManager.h"

#include <Externs.h>

#include <utility>

#include "DisplayManager.h"
#include "TextBox.h"
#include "default_bitmap_2bit.h"

WidgetManager::WidgetManager(DisplayManager *disp)
    : _dm(disp), _bg_image(0, 0, display.width(), display.height()) {
  _widgets.reserve(16);
}

void WidgetManager::draw() {
  _dm->setFullWindow();
  _dm->firstPage();
  do {
    this->drawBackground();
    this->drawWidgets();
  } while (_dm->nextPage());

  // _dm->display();
}

void WidgetManager::partDraw() {}

void WidgetManager::drawWidgets() {
  for (auto &widget : _widgets) {
    widget->tick();
    widget->draw(_dm);
  }
}

void WidgetManager::drawBackground() {
  // _dm->setFullWindow();
  // _dm->fillRect(0, 0, _dm->width(), _dm->height(), GxEPD_WHITE);
  _bg_image.draw(&display);
}

void WidgetManager::setBackground(const char *path) {}

void WidgetManager::addWidget(std::shared_ptr<Widget> widget) {
  _widgets.push_back(std::move(widget));
}

void WidgetManager::addWidget(Widget *widget) {
  _widgets.push_back(std::shared_ptr<Widget>(widget));
}

void WidgetManager::removeWidget(const std::shared_ptr<Widget> &widget) {
  for (auto it = _widgets.begin(); it != _widgets.end(); ++it) {
    if (it->get() == widget.get()) {
      _widgets.erase(it);
      return;
    }
  }
}

void WidgetManager::removeWidget(Widget *widget) {
  for (auto it = _widgets.begin(); it != _widgets.end(); ++it) {
    if (it->get() == widget) {
      _widgets.erase(it);
      return;
    }
  }
}

void WidgetManager::eraseWidgets() {
  _widgets.clear();
}

bool WidgetManager::saveScreenToLFS(uint8_t slot) {
  if (saveManager.saveScreen(slot, &_widgets, &_bg_image) != RESULTS::NO_ERROR) {
    return false;
  }

  if (saveManager.saveConfig() == RESULTS::NO_ERROR)
    LOG_LV1("WGTMNG", "Successfully saved config file");
  else
    LOG_LV1("WGTMNG", "Failed to save config file");
  return true;
}

bool WidgetManager::loadScreenFromLFS(uint8_t slot) {
  // this->draw();
  return saveManager.loadScreen(slot, &_widgets, &_bg_image) == RESULTS::NO_ERROR;
  ;
}

bool WidgetManager::loadLastScreen() {
  if (saveManager.loadScreen(settings.last_screen, &_widgets, &_bg_image) != RESULTS::NO_ERROR) {
    loadDefaultScreen();
    return false;
  }
  return true;
}

void WidgetManager::loadDefaultScreen() {
  saveManager.loadDefaultScreen(&_widgets, &_bg_image);
  display.setFullWindow();
  // this->draw();
}

BackgroundImage *WidgetManager::getBackground() {
  return &_bg_image;
}
