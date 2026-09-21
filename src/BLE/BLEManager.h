#ifndef EINK_BLUETOOTHMANAGER_H
#define EINK_BLUETOOTHMANAGER_H
#include <bluefruit.h>
#include <memory>
#include <Externs.h>
// class Widget;
// class WidgetManager;
// class SaveManager;
// class TextBox;
// class BackgroundImage;


// Battery Service
// UUID16_SVC_BATTERY 		// Battery Service
// UUID16_CHR_BATTERY_LEVEL // Bat level character

// Device configuration Service
#define UUID128_SVC_CONFIG   /** Config Service		**/	"735f66dd-bee5-4a54-8dcb-0aff8996bcf0"
#define UUID128_CHR_CFG_CMD  /** Character Command 	**/ "735f66dd-bee5-4a54-8dcb-0aff8996bcf1"
#define UUID128_CHR_CFG_STS  /** Character Status 	**/ "735f66dd-bee5-4a54-8dcb-0aff8996bcf2"
#define UUID128_CHR_CFG_DAT  /** Character Data 	**/ "735f66dd-bee5-4a54-8dcb-0aff8996bcf3"

#define BLE_BUFFER_SIZE       1024
#define BLE_CHUNK_PAYLOAD     248

class BLEManager
{
public:
	void begin(WidgetManager* widgetManager, SaveManager* saveManager);
	bool isConnected();
	bool isBusy();
	/**
	 * @brief Process command from ble client
	 * @param cmd command
	 * @param param command params
	 */
	void processCommand(uint8_t cmd, uint8_t param);

	/**
	 * @brief Process chunk of data from ble client
	 * @param data Pointer to data
	 * @param len Data length
	 */
	void processDataChunk(uint8_t* data, uint16_t len);

	/**
	 * @brief Send chunk of data to ble client
	 * @param type Data type (0x01 = Widget, 0xff = Background)
	 * @param chunk_num Number of chunk
	 * @param total_chunks Total chunks
	 * @param data Pointer to data
	 * @param data_len Data length
	 * @param is_last last chunk
	 */
	void sendDataChunk(uint8_t type, uint8_t chunk_num, uint16_t total_chunks,
	                   const uint8_t* data, uint16_t data_len, bool is_last);

	/**
	 * @brief Set operation status
	 * @param status status code
	 */
	void setStatus(uint8_t status);

	/**
	 * @brief Calc CRC16
	 * @param data Pointer to data
	 * @param len Data length
	 * @return CRC16 значение
	 */
	uint16_t calculateCRC16(const uint8_t* data, uint16_t len);

private:
	BLEService _svcBattery;
	BLECharacteristic _charBattery;

	BLEService _svcConfig; // configuration
	BLECharacteristic _charCommand; // commands for device
	BLECharacteristic _charStatus; // status of device
	BLECharacteristic _charData; // raw data like bitmap and config

	// uint8_t _buffer[BLE_BUFFER_SIZE]; // 1KB static buffer
	// size_t _buffer_size = 0;

	// Static receive buffer (keep small)
	static constexpr uint16_t BLE_RX_BUF_SIZE = 512;
	uint8_t _rxBuf[BLE_RX_BUF_SIZE] = {};

	// Receive state
	WidgetTypes _rxType = WidgetTypes::NONE;
	uint8_t _rxIndex = 0; // widget index or 0 for background
	uint16_t _rxTotalChunks = 0;
	uint16_t _rxReceivedChunks = 0;
	uint32_t _rxOffset = 0; // current offset in target buffer
	uint16_t _rxExpectedCrc = 0;

	// Temporary header storage for widgets
	widget_t _rxHeader{};
	text_box_t _rxTextBox{};
	char _rxText[TEXT_BUFFER_SIZE]{};
	bool _rxHeaderReceived = false;

	bool _busy = false;

	Widget* _widget = nullptr;
	void setupAdvertising();

	// Callbacks
	static void connect_callback(uint16_t conn_handle);
	static void disconnect_callback(uint16_t conn_handle, uint8_t reason);
	static void secured_callback(uint16_t conn_handle);
	static bool pairing_passkey_callback(uint16_t conn_hdl, uint8_t const passkey[6], bool match_request);
	static void pairing_complete_callback(uint16_t conn_hdl, uint8_t auth_status);

	void handleSaveWidgets();
	void handleLoadWidgets();
	void handleClearWidgets();
	void handleSaveBackground();
	void handleLoadBackground();
	void handleSendWidget(uint8_t index);
	void handleRecvWidget(uint8_t index);
	void handleSendBackground();
	void handleRecvBackground();
};


#endif //EINK_BLUETOOTHMANAGER_H
