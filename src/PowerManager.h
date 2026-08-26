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
	virtual void power_off();
	virtual void add_wake_pin(uint8_t pin, uint8_t reason);
	virtual void print_reset_reason();
};

class NRFPowerManager : PowerManagerBase
{
public:
	NRFPowerManager() = default;
	~NRFPowerManager() override = default;

	void power_off() override;
	void add_wake_pin(uint8_t pin, uint8_t reason) override;
	void print_reset_reason() override;

private:
	static std::set<irq_pin_cfg> _wake_pins;
};


#endif //EINK_NRFPOWERMANAGER_H
