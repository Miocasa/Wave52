//
// Created by miocasa on 9/13/26.
//

#ifndef EINK_SETTINGS_H
#define EINK_SETTINGS_H
#include <cstdint>

#define PATH_MAX_LENGTH 256

struct settings_t
{
	uint32_t wfi_timeout;
	uint32_t poweroff_timeout;

	bool partial_update_enabled;
	bool rtc_enabled;
	bool placeHolder[6]; // needs to fill bool flags to byte (round)

	char screen_path[PATH_MAX_LENGTH];
};
#endif //EINK_SETTINGS_H
