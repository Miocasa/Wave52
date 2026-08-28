#pragma once


#define Weact4_2_2bit
// #define GxEPD2_420_GDEY042T81 // analog define for Weact4_2_2bit

#define SEC_TO_MS(s) (s * 1000)

// --- E-ink pins---
#define EPD_BUSY_PIN   D9
#define EPD_RST_PIN    D7
#define EPD_DC_PIN     PIN_NFC2
#define EPD_CS_PIN     PIN_NFC1

// --- I2C pins ---
// #define SDA D4
// #define SCL D5


// --- Wake interuptions ---
#define WAKE_BUTTON_PIN   D0
#define WAKE_DS3231_PIN   D1

// --- Sleep ---
#define SLEEP_DELAY_MS SEC_TO_MS(30)
