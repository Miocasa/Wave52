#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_TinyUSB.h>
#include <ctime>
#include <GxEPD2_4G_BW.h>
#include <GxEPD2_4G_4G.h>
#include <bits/locale_classes.h>

#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/Picopixel.h>
#include <Fonts/minecraft_enchantment8pt7b.h>

#include "nrf_power.h"
#include "nrf_gpio.h"
#include "config.h"
#include <RTClib.h>

#include "ExternalRTC.h"
#include "PowerManager.h"

// WeAct Studio 4.2" = GDEY042T81, SSD1683, 400x300
GxEPD2_4G_4G<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT>
display(GxEPD2_420_GDEY042T81(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN));

ExternalRTC ex_rtc;
NRFPowerManager power;
void showGrayGradient();

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

	attachInterrupt(WAKE_BUTTON_PIN, button_irq, FALLING);
	attachInterrupt(WAKE_DS3231_PIN, ds3231_irq, FALLING);
	power.add_wake_pin(WAKE_BUTTON_PIN, FALLING);
	power.add_wake_pin(WAKE_DS3231_PIN, FALLING);

	// uint32_t t0 = millis();
	// while (!Serial && (millis() - t0) < 3000)
	// {
	// }

	pinMode(WAKE_BUTTON_PIN, INPUT_PULLUP);

	ex_rtc.begin();

	configure_nfc_pins();

	SPI.begin();

	display.init(115200, true, 50, false);
	display.setRotation(0);

	showGrayGradient();
	// showTimePartial();
}

void loop()
{
	char cmd = Serial.read();
	if (cmd == 'b')
	{
		enterUf2Dfu();
		// enterSerialDfu();
	}
	if (cmd == 'r')
	{
		NVIC_SystemReset();
		// enterOTADfu();
	}
	if (btn_st)
	{
		// prv_time = now;
		btn_st = false;
		showGrayGradient();
	}
	static uint32_t prv_time = millis();
	uint32_t now = millis();
	if (now - prv_time > SLEEP_DELAY_MS)
	{
		power.power_off();
	}
}


void showGrayGradient()
{
	display.setTextColor(GxEPD_BLACK);
	display.setFont(&FreeSansBold12pt7b);
	display.setTextSize(1);
	DateTime now = ex_rtc.get_time();
	char buf[30];
	sprintf(buf, "Time %02d:%02d:%02d", now.hour(), now.minute(), now.second());

	uint16_t tbw, tbh;
	int16_t tbx, tby;
	display.getTextBounds(buf, 0, 0, &tbx, &tby, &tbw, &tbh);
	int16_t x = (display.width() - tbw) / 2 - tbx;
	int16_t y = (display.height() - tbh) / 2 - tby;

	display.setFullWindow();
	display.firstPage();

	do
	{
		display.fillScreen(GxEPD_WHITE);
		display.setCursor(x, y);
		display.print(buf);
		// display.print("XIAO nRF52 2bit demo");
	}
	while (display.nextPage());

	display.hibernate();
}

