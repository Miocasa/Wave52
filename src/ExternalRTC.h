//
// Created by miocasa on 8/25/26.
//

#ifndef EINK_EXTERNALRTC_H
#define EINK_EXTERNALRTC_H
#include "RTClib.h"


class ExternalRTC
{
public:
	ExternalRTC() = default;
	~ExternalRTC() = default;

	void begin();

	DateTime get_time();
	bool set_time(DateTime time);
	void set_alarm(uint16_t num, DateTime time); // for ds3231 available 2 timers, 1 and 2.
	bool ready();

private:
	bool _rtc_ready = false;
	RTC_DS3231 _rtc;
};


#endif //EINK_EXTERNALRTC_H
