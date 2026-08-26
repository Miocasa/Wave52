#include "PowerManager.h"


void PowerManagerBase::power_off()
{
}

void PowerManagerBase::add_wake_pin(uint8_t pin, uint8_t reason)
{
}

void PowerManagerBase::print_reset_reason()
{
}

std::set<irq_pin_cfg> NRFPowerManager::_wake_pins;

void NRFPowerManager::power_off()
{
	LOG_LV1("Power", "Going to sleep now... (System OFF)");
	delay(50);

	for (const auto& pin : _wake_pins)
		nrf_gpio_cfg_sense_input(
			g_ADigitalPinMap[pin.pin],
			NRF_GPIO_PIN_PULLUP,
			pin.reason == FALLING ? NRF_GPIO_PIN_SENSE_LOW : NRF_GPIO_PIN_SENSE_HIGH
		);

	NRF_POWER->SYSTEMOFF = 1;

	while (1) { __WFE(); }
}

void NRFPowerManager::add_wake_pin(uint8_t pin, uint8_t reason)
{
	_wake_pins.insert(irq_pin_cfg{pin, reason});
}

void NRFPowerManager::print_reset_reason()
{
	uint32_t reason = readResetReason();

	Serial.print("RESETREAS = 0x");
	Serial.println(reason, HEX);

	if (reason & POWER_RESETREAS_OFF_Msk)
	{
		Serial.println(">>> Woke up by GPIO (button) from System OFF!");
	}
	else if (reason & POWER_RESETREAS_RESETPIN_Msk)
	{
		Serial.println(">>> Reset by RESET pin.");
	}
	else if (reason == 0)
	{
		Serial.println(">>> Power-on reset (первое включение / USB).");
	}
	else
	{
		Serial.println(">>> Other reset reason.");
	}

	NRF_POWER->RESETREAS = reason;
}
