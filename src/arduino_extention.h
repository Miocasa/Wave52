#pragma once
#include <Arduino.h>

inline uint32_t tick_diff(uint32_t current_ticks, uint32_t previous_ticks) {
  if (current_ticks < previous_ticks)
    return UINT32_MAX - previous_ticks + current_ticks + 1;
  return current_ticks - previous_ticks;
}

/* --- Macros --- */
#define SEC_TO_MS(s) (s * 1000)
#define MIN_TO_MS(s) (s * 1000 * 1000)
