//
// Created by miocasa on 9/15/26.
//

#ifndef EINK_MBUTTON_H
#define EINK_MBUTTON_H
#include <Arduino.h>

typedef void (*mbutton_cb)(bool state);

class MButton

{
 public:
  MButton(int8_t pin = -1, uint8_t dwMode = INPUT_PULLUP, uint8_t active_level = LOW);

  void setPin(int8_t pin, uint8_t dwMode = INPUT_PULLUP, uint8_t active_level = LOW);

  void setDebounce(uint16_t debounce);    // anti debounce timeout
  void setClickTimeout(uint16_t timeout); // between clicks

  void setCallback(mbutton_cb callback);

  void tick(); // call in loop() or yeald

  // Button statuses, don't forget to call reset() or resetClicks after every state read.
  bool isClicked(); // press and release

  bool isDoubleClick(); // double click
  bool isTripleClick();
  // bool isHold();

  void reset();      // reset all states
  void resetClick(); // reset clicks count to 0

  uint8_t getClicks() const;

  // Callbacks
  void _btn_callback();

 private:
  int8_t _pin = -1;
  uint8_t _dwMode = INPUT_PULLUP;
  uint8_t _active_level = LOW;

  uint8_t _clicks_count = 0;
  uint8_t _final_clicks = 0;

  // timeouts
  uint16_t _debounce = 80;
  uint16_t _clickTimeout = 300;
  // uint16_t _clickResetTimeout;
  // uint16_t _holdTimeout;

  uint32_t _event_time = 0; // ticks on last event

  bool _has_click = false;

  bool _btn_st = false;     // real time state without filters
  bool _prv_btn_st = false; // previous state

  // bool _pressed = false; // current state with debounce
  // bool _clicked = false; // click state
  // bool _doubleClick = false; // double click
};

#endif // EINK_MBUTTON_H
