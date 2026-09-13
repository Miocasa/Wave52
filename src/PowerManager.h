#pragma once
#ifndef EINK_NRFPOWERMANAGER_H
#define EINK_NRFPOWERMANAGER_H
#include <Arduino.h>
#include <bluefruit.h>
#include <set>

struct irq_pin_cfg
{
	uint8_t pin;
	uint8_t reason;

	bool operator<(const irq_pin_cfg& other) const
	{
		return pin < other.pin;
	}

	bool operator==(const irq_pin_cfg& other) const
	{
		return pin == other.pin && reason == other.reason;
	}
};

class PowerManagerBase
{
public:
	PowerManagerBase() = default;
	virtual ~PowerManagerBase() = default;

	virtual void add_wake_pin(uint8_t pin, uint8_t reason) = 0;
	virtual void print_reset_reason() = 0;
	virtual void power_off() = 0;
	virtual void reset() = 0;
	virtual void enterBootloader() = 0;
};
#ifdef ARDUINO_ARCH_NRF52
class NRFPowerManager : PowerManagerBase
{
public:
	NRFPowerManager() = default;
	~NRFPowerManager() override = default;


	void add_wake_pin(uint8_t pin, uint8_t reason) override;
	void print_reset_reason() override;

	void power_off() override;
	void reset() override;
	void enterBootloader() override;

private:
	static std::set<irq_pin_cfg> _wake_pins;
};
#endif


#endif //EINK_NRFPOWERMANAGER_H
