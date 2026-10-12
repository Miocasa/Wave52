#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <GxEPD2_4G_4G.h>
#include <GxEPD2_4G_BW.h>
#include <MButton.h>
#include <RTClib.h>
#include <SPI.h>
#include <bits/locale_classes.h>

#include <ctime>

#include "BLE/BLEManager.h"
#include "DisplayManager.h"
#include "ExternalRTC.h"
#include "Externs.h"
#include "PowerManager.h"
#include "arduino_extention.h"
#include "config.h"
#include "nrf_gpio.h"
#include "nrf_power.h"

MButton button(D0);
DisplayManager display(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN);
DisplayManager_BW display_bw(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN);

ExternalRTC externalRTC;
WidgetManager widgetManager(&display);
SaveManager saveManager;
settings_t settings;
BLEManager bleManager;

bool rtc_st = false;

void ds3231_irq() {
  rtc_st = true;
}

bool configure_nfc_pins() {
  if ((NRF_UICR->NFCPINS & UICR_NFCPINS_PROTECT_Msk) ==
      (UICR_NFCPINS_PROTECT_NFC << UICR_NFCPINS_PROTECT_Pos)) {
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
    }

    NRF_UICR->NFCPINS &= ~UICR_NFCPINS_PROTECT_Msk;

    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
    }

    NVIC_SystemReset();
  }
  return false;
}

void setup() {
#if CFG_DEBUG
  Serial.begin(115200);
  delay(200);
#endif

  // pinMode(WAKE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(WAKE_DS3231_PIN, INPUT_PULLUP);
  // attachInterrupt(WAKE_BUTTON_PIN, button_irq, FALLING);
  attachInterrupt(WAKE_DS3231_PIN, ds3231_irq, FALLING);
  NRFPowerManager::add_wake_pin(WAKE_BUTTON_PIN, FALLING);
  NRFPowerManager::add_wake_pin(WAKE_DS3231_PIN, FALLING);

  externalRTC.begin();

  configure_nfc_pins();

  saveManager.begin();
  bleManager.begin(&widgetManager, &saveManager);
  widgetManager.loadLastScreen();

  SPI.begin();
#if CFG_DEBUG
  display.init(115200, true, 50, false);
#else
  display.init(0, true, 50, false);
#endif
  display.setRotation(0);

  button.reset(); // prevent double refresh, if it wakes from deep sleep by button
  widgetManager.draw();
}

void loop() {
  button.tick();
  static uint32_t prv_time = millis();
  uint32_t now = millis();

#if CFG_DEBUG
  static String path;
  char cmd = Serial.read();
  switch (cmd) {
    case 'o':
      NRFPowerManager::power_off();
      break;
    case 'b':
      NRFPowerManager::enterBootloader();
      break;
    case 'r':
      NRFPowerManager::reset();
      break;
    case 's':
      Serial.println("Saving...");

      if (widgetManager.saveScreenToLFS(0))
        Serial.println("Saved to slot 0 successfully");
      else
        Serial.println("Failed to save to slot 0");
      prv_time = now;
      break;
    case 'l':
      Serial.println("Loading...");
      path = "/v1_test/";
      if (widgetManager.loadScreenFromLFS(0))
        Serial.println("Loaded to slot 0 successfully");
      else
        Serial.println("Failed to load from slot 0");
      prv_time = now;
      break;
    case 'd':
      // widgetManager.test();
      widgetManager.loadDefaultScreen();
      break;

    case 't': {
      static uint8_t ind = 0;
      Serial.println(F("Partial test..."));

      const uint16_t bx = 8;
      const uint16_t by = 8;
      const uint16_t bw = 64; // multiple of 8
      const uint16_t bh = 32;

      display_bw.setPartialWindow(bx, by, bw, bh);
      display_bw.firstPage();
      do {
        display_bw.fillRect(bx, by, bw, bh, GxEPD_WHITE);
        display_bw.setCursor(bx + 4, by + 20);
        display_bw.setTextColor(GxEPD_BLACK);
        display_bw.setFont(nullptr);
        display_bw.setTextSize(1);
        display_bw.printf("part %d", ind);
      } while (display_bw.nextPage());
      ind++;

      Serial.println(F("Partial done"));
      break;
    }
    default:
      break;
  }

  if (rtc_st)
    Serial.println("DS3231 IRQ");
  // if (btn_st) Serial.println("Button IRQ");
#endif

  if (button.isClicked()) {
    prv_time = now;
    rtc_st = false;

    display.update();
  }
  // Bit
  if (bleManager.isConnected()) {
    prv_time = now;
  }

  if (tick_diff(now, prv_time) > settings.wfi_timeout) {
    // power.power_off();
    NRFPowerManager::sleep();
  }
}