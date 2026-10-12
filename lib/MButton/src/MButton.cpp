//
// Created by miocasa on 9/15/26.
//

#include "MButton.h"

inline uint32_t tick_diff(uint32_t current_ticks, uint32_t previous_ticks) {
  if (current_ticks < previous_ticks)
    return UINT32_MAX - previous_ticks + current_ticks + 1;
  return current_ticks - previous_ticks;
}

MButton* g_mbutton = nullptr;

MButton::MButton(int8_t pin, uint8_t dwMode, uint8_t active_level) {
  setPin(pin, dwMode, active_level);
  g_mbutton = this;
}

void MButton::tick() {
  if (_clicks_count > 0) {
    uint32_t delta = tick_diff(millis(), _event_time);
    if (delta > _clickTimeout) {
      _final_clicks = _clicks_count;
      _clicks_count = 0;
      _has_click = true;
    }
  }
}

void _irq_handler() {
  g_mbutton->_btn_callback();
}

void MButton::setPin(int8_t pin, uint8_t dwMode, uint8_t active_level) {
  this->_pin = pin;
  this->_dwMode = dwMode;
  this->_active_level = active_level;
  pinMode(pin, dwMode);
  attachInterrupt(digitalPinToInterrupt(_pin), _irq_handler, CHANGE);
}

void MButton::setDebounce(uint16_t debounce) {
  this->_debounce = debounce;
}

void MButton::setClickTimeout(uint16_t timeout) {
  this->_clickTimeout = timeout;
}

// void MButton::setClickResetTimeout(uint16_t timeout)
// {
// 	this->_clickResetTimeout = timeout;
// }

// void MButton::setHoldTimeout(uint16_t timeout)
// {
// 	this->_holdTimeout = timeout;
// }

void MButton::setCallback(mbutton_cb callback) {
  // todo implement later
}

bool MButton::isClicked() {
  if (_has_click && _final_clicks >= 1) {
    _has_click = false;
    return true;
  }
  return false;
}

bool MButton::isDoubleClick() {
  if (_has_click && _final_clicks >= 2) {
    _has_click = false;
    return true;
  }
  return false;
}

bool MButton::isTripleClick() {
  if (_has_click && _final_clicks >= 3) {
    _has_click = false;
    return true;
  }
  return false;
}

void MButton::reset() {
  resetClick();
}

void MButton::resetClick() {
  _clicks_count = 0;
  _final_clicks = 0;
  _has_click = false;
}

uint8_t MButton::getClicks() const {
  return _clicks_count;
}

void MButton::_btn_callback() {
  bool st = digitalRead(_pin);
  if (st == _prv_btn_st)
    return;

  uint32_t now = millis();
  uint32_t delta = tick_diff(now, _event_time);

  if (delta < _debounce)
    return;

  _prv_btn_st = st;
  _event_time = now;

  if (st != _active_level) {
    if (delta <= _clickTimeout)
      _clicks_count++;
    else
      _clicks_count = 1;
  }
}
