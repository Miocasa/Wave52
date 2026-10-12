#pragma once
#ifndef EINK_NRFPOWERMANAGER_H
#define EINK_NRFPOWERMANAGER_H
#include <Arduino.h>
#include <bluefruit.h>

#include <set>

struct irq_pin_cfg {
  uint8_t pin;
  uint8_t reason;

  bool operator<(const irq_pin_cfg& other) const {
    return pin < other.pin;
  }

  bool operator==(const irq_pin_cfg& other) const {
    return pin == other.pin && reason == other.reason;
  }
};

#ifdef ARDUINO_ARCH_NRF52
namespace NRFPowerManager {
  void setup_battery();
  uint16_t read_battery_mv();
  uint8_t read_battery();
  void disconnectBle();
  void ble_sleep();

  void add_wake_pin(uint8_t pin, uint8_t reason);
  void print_reset_reason();
  void sleep();
  void power_off();
  void reset();
  void enterBootloader();
}; // namespace NRFPowerManager
#endif

#endif // EINK_NRFPOWERMANAGER_H
