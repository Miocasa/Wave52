#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <MButton.h>

#include "arduino_extention.h"
#include "config.h"

auto button = MButton(D0);

void setup() {
  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && tick_diff(millis(), t0) > SEC_TO_MS(3)) {
    delay(20);
  }

  delay(300);

  Serial.println("Initializing...");
}

void loop() {
  button.tick();

  if (button.isTripleClick())
    Serial.println("Triple Click");
  else if (button.isDoubleClick())
    Serial.println("Double Click");
  else if (button.isClicked())
    Serial.println("Clicked");
}
