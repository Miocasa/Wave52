//
// Created by miocasa on 9/13/26.
//

#ifndef EINK_CMDPARSER_H
#define EINK_CMDPARSER_H
#include <Arduino.h>

enum : uint8_t
{
	LOAD_CONFIG_FROM_FS_CMD = 0x01,
	SAVE_CONFIG_TO_FS_CMD = 0x02,

	LOAD_CONFIG_FROM_BLE_CMD = 0x03,
	SAVE_CONFIG_FROM_BLE_CMD = 0x04,
	SAVE_WIDGET_CMD = 0x05,
	LOAD_WIDGET_CMD = 0x06,
	SAVE_BACKGROUND = 0x07,
	LOAD_BACKGROUND = 0x08,
} COMMANDS;

enum : uint8_t
{
	IDLE_ST = 0x00,
	UPDATING_EINK_ST = 0x01,
	LOADING_CFG_ST = 0x02,
	SAVING_CFG_ST = 0x03,
} DEVICE_STATUS;


class CmdParser
{
public:
	CmdParser();
	~CmdParser();

private:
};


#endif //EINK_CMDPARSER_H
