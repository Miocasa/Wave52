#ifndef EINK_BLUETOOTHMANAGER_H
#define EINK_BLUETOOTHMANAGER_H
#include <bluefruit.h>

// Battery Service
// UUID16_SVC_BATTERY 		// Battery Service
// UUID16_CHR_BATTERY_LEVEL // Bat level character

// Device configuration Service
#define UUID128_SVC_CONFIG   /** Config Service		**/	"735f66dd-bee5-4a54-8dcb-0aff8996bcj0"
#define UUID128_CHR_CFG_CMD  /** Character Command 	**/ "735f66dd-bee5-4a54-8dcb-0aff8996bcj1"
#define UUID128_CHR_CFG_STS  /** Character Status 	**/ "735f66dd-bee5-4a54-8dcb-0aff8996bcj2"
#define UUID128_CHR_CFG_DAT  /** Character Data 	**/ "735f66dd-bee5-4a54-8dcb-0aff8996bcj3"

class BLEManager
{
public:
	void begin();
	bool isConnected();

private:
	BLEService _svcBattery;
	BLECharacteristic _charBattery;

	BLEService _svcConfig; // configuration
	BLECharacteristic _charCommand; // commands for device
	BLECharacteristic _charStatus; // status of device
	BLECharacteristic _charData; // raw data like bitmap and config

	// Callbacks
	static void connect_callback(uint16_t conn_handle);
	static void disconnect_callback(uint16_t conn_handle, uint8_t reason);
	static void secured_callback(uint16_t conn_handle);
	void setupAdvertising();
};


#endif //EINK_BLUETOOTHMANAGER_H
