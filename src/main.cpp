#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_TinyUSB.h>
#include <ctime>
#include <GxEPD2_4G_BW.h>
#include <GxEPD2_4G_4G.h>
#include <bits/locale_classes.h>

#include "nrf_power.h"
#include "nrf_gpio.h"
#include "config.h"
#include <RTClib.h>

#include "DisplayManager.h"
#include "ExternalRTC.h"
#include "Externs.h"
#include "PowerManager.h"

DisplayManager display(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN);
ExternalRTC externalRTC;
NRFPowerManager power;
WidgetManager widgetManager(&display);
SaveManager saveManager;

bool btn_st = false;

void button_irq()
{
	Serial.println("Button IRQ");
	btn_st = true;
}

void ds3231_irq()
{
	Serial.println("DS3231 IRQ");
}

bool configure_nfc_pins()
{
	if ((NRF_UICR->NFCPINS & UICR_NFCPINS_PROTECT_Msk) == (UICR_NFCPINS_PROTECT_NFC << UICR_NFCPINS_PROTECT_Pos))
	{
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
		{
		}

		NRF_UICR->NFCPINS &= ~UICR_NFCPINS_PROTECT_Msk;

		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
		{
		}

		NVIC_SystemReset();
	}
	return false;
}


void setup()
{
	Serial.begin(115200);
	delay(200);

	pinMode(WAKE_BUTTON_PIN, INPUT_PULLUP);
	pinMode(WAKE_DS3231_PIN, INPUT_PULLUP);
	attachInterrupt(WAKE_BUTTON_PIN, button_irq, FALLING);
	attachInterrupt(WAKE_DS3231_PIN, ds3231_irq, FALLING);
	power.add_wake_pin(WAKE_BUTTON_PIN, FALLING);
	power.add_wake_pin(WAKE_DS3231_PIN, FALLING);

	// uint32_t t0 = millis();
	// while (!Serial && (millis() - t0) < 3000)
	// {
	// }

	externalRTC.begin();

	configure_nfc_pins();

	saveManager.begin();

	SPI.begin();
	display.init(115200, true, 50, false);
	display.setRotation(0);

	display.update();
	// showTimePartial();
}

void loop()
{
	static uint32_t prv_time = millis();
	uint32_t now = millis();
	static String path;

	char cmd = Serial.read();
	switch (cmd)
	{
	case 'b':
		power.enterBootloader();
		break;
	case 'r':
		power.reset();
		break;
	case 's':
		Serial.println("Saving...");
		path = "/v1_test/";
		if (widgetManager.saveScreenToLFS(path.c_str()))
			Serial.println("Saved to \"" + path + "\" successfully");
		else
			Serial.println("Failed to save \"" + path);
		prv_time = now;
		break;
	case 'l':
		Serial.println("Loading...");
		path = "/v1_test/";
		if (widgetManager.loadScreenFromLFS(path.c_str()))
			Serial.println("Loaded to \"" + path + "\" successfully");
		else
			Serial.println("Failed to load \"" + path);
		prv_time = now;
		break;
	default: break;
	}

	if (btn_st)
	{
		prv_time = now;
		btn_st = false;
		display.update();
	}


	if (now - prv_time > SLEEP_DELAY_MS)
	{
		power.power_off();
	}
}



