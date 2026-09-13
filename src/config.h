#pragma once

/**
 * @file config.h
 * @brief General configs, most of them can be redefined
 */

/* --- Macros --- */
#define SEC_TO_MS(s) (s * 1000)

/**
 * @brief Display type definition, now support only WeAct 4.2 2bit color depth (Weact4_2_2bit)
 */
#define Weact4_2_2bit


/* ---  Default values --- */
#define WFI_TIMEOUT SEC_TO_MS(10) // not implemented yet
#define SLEEP_TIMEOUT SEC_TO_MS(30)

// --- Constants ---
#define DEVICE_NAME "Wave52" // BLE name
#define CONFIG_PATH "/config.bin"

/* --- SPI/E-ink pins--- */
#define EPD_MOSI_PIN   D10
#define EPD_MISO_PIN   D9
#define EPD_SCK_PIN    D8
#define EPD_BUSY_PIN   D9 // TODO change pin it conflict with EPD_MISO_PIN
#define EPD_RST_PIN    D7
#define EPD_DC_PIN     PIN_NFC2
#define EPD_CS_PIN     PIN_NFC1

// --- I2C pins ---
// #define SDA D4
// #define SCL D5

// --- Wake interruption pins ---
#define WAKE_BUTTON_PIN   D0
#define WAKE_DS3231_PIN   D1


