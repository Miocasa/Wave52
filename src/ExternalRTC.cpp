//
// Created by miocasa on 8/25/26.
//

#include "ExternalRTC.h"

void ExternalRTC::begin()
{
	if (!_rtc.begin())
	{
		LOG_LV1("RTC", "Couldn't find RTC!");
	}

	if (_rtc.lostPower())
	{
		LOG_LV1("RTC", "Lost power, set buildtime.");
		_rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	}

	_rtc.disable32K();

	if (_rtc.alarmFired(1)) _rtc.clearAlarm(1);
	if (_rtc.alarmFired(2)) _rtc.clearAlarm(2);
	LOG_LV1("RTC", "Alarms cleared");

	_rtc.writeSqwPinMode(DS3231_OFF);

	_rtc.disableAlarm(1);
	_rtc.disableAlarm(2);

	_rtc_ready = true;
	LOG_LV1("RTC", "RTC ready");
}

bool ExternalRTC::ready() { return _rtc_ready; };

auto ExternalRTC::get_time() -> DateTime
{
	return _rtc_ready ? _rtc.now() : DateTime();
}

bool ExternalRTC::set_time(DateTime time)
{
	if (!_rtc_ready) return false;
	_rtc.adjust(time);
	return true;
}

void ExternalRTC::set_alarm(uint16_t num, DateTime time)
{
	if (num == 1)
	{
		_rtc.setAlarm1(time, DS3231_A1_Date);
		return;
	}
	if (num == 2)
	{
		_rtc.setAlarm2(time, DS3231_A2_Date);
	}
}
