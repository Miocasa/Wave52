//
// Created by miocasa on 9/12/26.
//

#include "BLEManager.h"

#include "config.h"

void BLEManager::begin()
{
	Bluefruit.configAttrTableSize(2048);
	// Bluefruit.configUuid128Count(20);
	Bluefruit.configPrphConn(247, 3, 2, 2);

	Bluefruit.begin();
	Bluefruit.setName(DEVICE_NAME);
	Bluefruit.setTxPower(4);
	Bluefruit.Periph.setConnectCallback(connect_callback);
	Bluefruit.Security.setSecuredCallback(secured_callback);

	// Battery service
	_svcBattery = BLEService(UUID16_SVC_BATTERY);
	_svcBattery.begin();

	// Bat percent characteristic (read)
	_charBattery = BLECharacteristic(UUID16_CHR_BATTERY_LEVEL);
	_charBattery.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
	_charBattery.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
	_charBattery.setFixedLen(1);
	_charBattery.begin();
	_charBattery.write8(100); // placeholder battery level


	// Config service
	_svcConfig = BLEService(UUID128_SVC_CONFIG);
	_svcConfig.begin();

	// CMD characteristic (write)
	_charCommand = BLECharacteristic(UUID128_CHR_CFG_CMD);
	_charCommand.setProperties(CHR_PROPS_WRITE);
	_charCommand.setPermission(SECMODE_NO_ACCESS, SECMODE_OPEN);
	_charCommand.setFixedLen(2);
	_charCommand.begin();

	// Status characteristic (read)
	_charStatus = BLECharacteristic(UUID128_CHR_CFG_STS);
	_charStatus.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
	_charStatus.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
	_charStatus.setFixedLen(1);
	_charStatus.begin();

	// Raw data characteristic
	_charData = BLECharacteristic(UUID128_CHR_CFG_DAT);
	_charData.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
	_charData.setPermission(SECMODE_OPEN, SECMODE_OPEN);
	_charData.setFixedLen(255);
	_charData.begin();

	setupAdvertising();
	Bluefruit.Advertising.start(0);
	LOG_LV1("BLE", "Advertising as \"%s\"", DEVICE_NAME);
}

bool BLEManager::isConnected()
{
	return Bluefruit.connected();
}


void BLEManager::connect_callback(uint16_t conn_handle)
{
	BLEConnection* conn = Bluefruit.Connection(conn_handle);
	char addr[18] = {0};
	ble_gap_addr_t peer = conn->getPeerAddr();
	snprintf(addr, sizeof(addr), "%02X:%02X:%02X:%02X:%02X:%02X",
	         peer.addr[5], peer.addr[4], peer.addr[3],
	         peer.addr[2], peer.addr[1], peer.addr[0]);
	LOG_LV1("BLE", "Connected to %s", addr);
}


void BLEManager::disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
	(void)conn_handle;
	LOG_LV1("BLE", "Disconnected, reason=0x%02X", reason);
}

void BLEManager::secured_callback(uint16_t conn_handle)
{
	LOG_LV1("BLE", "Secured connection");
}

void BLEManager::setupAdvertising()
{
	Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
	Bluefruit.Advertising.addTxPower();
	Bluefruit.Advertising.addName();

	Bluefruit.Advertising.addService(_svcBattery);
	Bluefruit.Advertising.addService(_svcConfig);

	Bluefruit.Advertising.restartOnDisconnect(true);
	Bluefruit.Advertising.setInterval(32, 244);
	Bluefruit.Advertising.setFastTimeout(30);
	// Bluefruit.Advertising.setInterval(160, 800);
	// Bluefruit.Advertising.setFastTimeout(20);
}
