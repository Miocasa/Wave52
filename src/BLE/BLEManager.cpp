//
// Created by miocasa on 9/12/26.
//

#include "BLEManager.h"

#include "config.h"
#include "Externs.h"
#include "My_Arduino_improves.h"
#include "Widgets/TextBox.h"
#include "Widgets/WidgetManager.h"

#define SAVE_WIDGETS_CMD      0x10
#define LOAD_WIDGETS_CMD      0x11
#define CLEAR_WIDGETS_CMD     0x12
#define SAVE_BACKGROUND_CMD   0x20
#define LOAD_BACKGROUND_CMD   0x21
#define BLE_SEND_WIDGET_CMD   0x30
#define BLE_RECV_WIDGET_CMD   0x31
#define BLE_SEND_BG_CMD       0x32
#define BLE_RECV_BG_CMD       0x33

#define STATUS_IDLE           0x00
#define STATUS_BUSY           0x01
#define STATUS_SUCCESS        0x02
#define STATUS_ERROR          0x03
#define STATUS_WAITING_DATA   0x04
#define STATUS_SENDING_DATA   0x05

#define CHUNK_HEADER_SIZE     7
#define CHUNK_PAYLOAD_MAX     248
#define BG_BUFFER_SIZE        30000
#define BG_DEFAULT_BPP        2

static BLEManager* g_bleManager = nullptr;
static WidgetManager* g_widgetManager = nullptr;
static SaveManager* g_saveManager = nullptr;

struct BleDataChunk
{
    uint8_t type;
    uint8_t chunk_num;
    uint16_t total_chunks;
    uint8_t flags;
    uint16_t crc;
    uint8_t data[248];
} __attribute__((packed));

static void on_command_write(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len)
{
    if (len < 2 || !g_bleManager) return;

    Serial.print("Command received: 0x");
    Serial.print(data[0], HEX);
    Serial.print(" param=");
    Serial.println(data[1]);

    g_bleManager->processCommand(data[0], data[1]);
}

static void on_data_write(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len)
{
    if (!g_bleManager || len < CHUNK_HEADER_SIZE) return;

    g_bleManager->processDataChunk(data, len);
}

void BLEManager::begin(WidgetManager* widgetManager, SaveManager* saveManager)
{
    g_bleManager = this;
    g_widgetManager = widgetManager;
    g_saveManager = saveManager;

    Bluefruit.configAttrTableSize(2048);
    Bluefruit.configPrphConn(247, 3, 2, 2);

    Bluefruit.begin();
    Bluefruit.setName(DEVICE_NAME);
    Bluefruit.setTxPower(4);

    Bluefruit.Security.setIOCaps(true, true, false);
    Bluefruit.Security.setMITM(true);
    Bluefruit.Security.setPairPasskeyCallback(pairing_passkey_callback);
    Bluefruit.Security.setPairCompleteCallback(pairing_complete_callback);

    Bluefruit.Periph.setConnectCallback(connect_callback);
    Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
    Bluefruit.Security.setSecuredCallback(secured_callback);

    _svcBattery = BLEService(UUID16_SVC_BATTERY);
    _svcBattery.begin();

    _charBattery = BLECharacteristic(UUID16_CHR_BATTERY_LEVEL);
    _charBattery.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
    _charBattery.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
    _charBattery.setFixedLen(1);
    _charBattery.begin();
    _charBattery.write8(100);

    _svcConfig = BLEService(UUID128_SVC_CONFIG);
    _svcConfig.begin();

    _charCommand = BLECharacteristic(UUID128_CHR_CFG_CMD);
    _charCommand.setProperties(CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP);
    _charCommand.setPermission(SECMODE_NO_ACCESS, SECMODE_OPEN);
    _charCommand.setFixedLen(2);
    _charCommand.setWriteCallback(on_command_write);
    _charCommand.begin();

    _charStatus = BLECharacteristic(UUID128_CHR_CFG_STS);
    _charStatus.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
    _charStatus.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
    _charStatus.setFixedLen(1);
    _charStatus.begin();
    _charStatus.write8(STATUS_IDLE);

    _charData = BLECharacteristic(UUID128_CHR_CFG_DAT);
    _charData.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP | CHR_PROPS_NOTIFY);
    _charData.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    _charData.setMaxLen(255);
    _charData.setWriteCallback(on_data_write);
    _charData.begin();

    setupAdvertising();
    Bluefruit.Advertising.start(0);

    Serial.println("BLE Manager initialized");
}

void BLEManager::processCommand(uint8_t cmd, uint8_t param)
{
    setStatus(STATUS_BUSY);

    switch (cmd)
    {
    case SAVE_WIDGETS_CMD:
        handleSaveWidgets();
        break;

    case LOAD_WIDGETS_CMD:
        handleLoadWidgets();
        break;

    case CLEAR_WIDGETS_CMD:
        handleClearWidgets();
        break;

    case SAVE_BACKGROUND_CMD:
        handleSaveBackground();
        break;

    case LOAD_BACKGROUND_CMD:
        handleLoadBackground();
        break;

    case BLE_SEND_WIDGET_CMD:
        handleSendWidget(param);
        break;

    case BLE_RECV_WIDGET_CMD:
        handleRecvWidget(param);
        break;

    case BLE_SEND_BG_CMD:
        handleSendBackground();
        break;

    case BLE_RECV_BG_CMD:
        handleRecvBackground();
        break;

    default:
        Serial.println("Unknown command");
        setStatus(STATUS_ERROR);
    }
}

void BLEManager::handleSaveWidgets()
{
    if (!g_widgetManager)
    {
        setStatus(STATUS_ERROR);
        return;
    }

    if (g_widgetManager->saveScreenToLFS("/widgets/"))
    {
        setStatus(STATUS_SUCCESS);
        Serial.println("Widgets saved to FS");
    }
    else
    {
        setStatus(STATUS_ERROR);
        Serial.println("Failed to save widgets");
    }
}

void BLEManager::handleLoadWidgets()
{
    if (!g_widgetManager)
    {
        setStatus(STATUS_ERROR);
        return;
    }

    if (g_widgetManager->loadScreenFromLFS("/widgets/"))
    {
        setStatus(STATUS_SUCCESS);
        Serial.println("Widgets loaded from FS");
    }
    else
    {
        setStatus(STATUS_ERROR);
        Serial.println("Failed to load widgets");
    }
}

void BLEManager::handleClearWidgets()
{
    if (!g_widgetManager)
    {
        setStatus(STATUS_ERROR);
        return;
    }

    g_widgetManager->eraseWidgets();
    setStatus(STATUS_SUCCESS);
    Serial.println("Widgets cleared");
}

void BLEManager::handleSaveBackground()
{
    setStatus(STATUS_SUCCESS);
}

void BLEManager::handleLoadBackground()
{
    setStatus(STATUS_SUCCESS);
}

void BLEManager::handleSendWidget(uint8_t index)
{
    setStatus(STATUS_SENDING_DATA);
    setStatus(STATUS_SUCCESS);
}

void BLEManager::handleRecvWidget(uint8_t index)
{
    _busy = true;
    _rxIndex = index;
    _rxType = WidgetTypes::NONE;
    _rxOffset = 0;
    _rxReceivedChunks = 0;
    _rxTotalChunks = 0;
    setStatus(STATUS_WAITING_DATA);
    Serial.println("Waiting for widget data");
}

void BLEManager::handleSendBackground()
{
    setStatus(STATUS_SENDING_DATA);
    setStatus(STATUS_SUCCESS);
}

void BLEManager::handleRecvBackground()
{
    _busy = true;
    _rxIndex = 0;
    _rxType = WidgetTypes::BG_IMAGE;
    _rxOffset = 0;
    _rxReceivedChunks = 0;
    _rxTotalChunks = 0;
    setStatus(STATUS_WAITING_DATA);
    Serial.println("Waiting for background data");
}

void BLEManager::processDataChunk(uint8_t* data, uint16_t len)
{
    if (len < CHUNK_HEADER_SIZE || !_busy) return;

    const uint8_t chunkType = data[0];
    const uint8_t chunkNum = data[1];
    const uint16_t totalChunks = ((uint16_t)data[2] << 8) | data[3];
    const uint8_t flags = data[4];
    const uint8_t* payload = data + CHUNK_HEADER_SIZE;

    if (totalChunks == 0 || chunkNum >= totalChunks)
    {
        Serial.println("Invalid chunk number");
        _busy = false;
        setStatus(STATUS_ERROR);
        return;
    }

    if (chunkNum == 0)
    {
        if (_rxType == WidgetTypes::NONE)
        {
            _rxType = (WidgetTypes)chunkType;
        }
        _rxTotalChunks = totalChunks;
        _rxReceivedChunks = 0;
        _rxOffset = 0;

        Serial.print("Transfer started: ");
        Serial.print(totalChunks);
        Serial.println(" chunks");
    }

    uint16_t payloadLen = len - CHUNK_HEADER_SIZE;
    if (payloadLen > CHUNK_PAYLOAD_MAX) payloadLen = CHUNK_PAYLOAD_MAX;
    if (payloadLen == 0) return;

    if (_rxType == WidgetTypes::BG_IMAGE)
    {
        BackgroundImage* bg = g_widgetManager ? g_widgetManager->getBackground() : nullptr;
        if (!bg)
        {
            _busy = false;
            setStatus(STATUS_ERROR);
            return;
        }

        if (chunkNum == 0)
        {
            bg->set_bpp(BG_DEFAULT_BPP);
            bg->clear(0xFF);
        }

        uint16_t writeLen = payloadLen;
        if (_rxOffset >= BG_BUFFER_SIZE)
        {
            writeLen = 0;
        }
        else if (_rxOffset + writeLen > BG_BUFFER_SIZE)
        {
            writeLen = BG_BUFFER_SIZE - _rxOffset;
        }

        if (writeLen > 0)
        {
            bg->set_bitmap_part(payload, _rxOffset, writeLen);
            _rxOffset += writeLen;
        }
    }
    else if (_rxType == WidgetTypes::TEXT_BOX)
    {
        if (_rxOffset + payloadLen > BLE_RX_BUF_SIZE)
        {
            Serial.println("Buffer overflow");
            _busy = false;
            setStatus(STATUS_ERROR);
            return;
        }

        memcpy(_rxBuf + _rxOffset, payload, payloadLen);
        _rxOffset += payloadLen;

        if (chunkNum + 1 == _rxTotalChunks)
        {
            if (_rxOffset < sizeof(widget_t) + sizeof(text_box_t))
            {
                _busy = false;
                setStatus(STATUS_ERROR);
                return;
            }

            memcpy(&_rxHeader, _rxBuf, sizeof(widget_t));
            memcpy(&_rxTextBox, _rxBuf + sizeof(widget_t), sizeof(text_box_t));

            size_t textLen = _rxTextBox._size;
            if (textLen >= TEXT_BUFFER_SIZE) textLen = TEXT_BUFFER_SIZE - 1;
            if (sizeof(widget_t) + sizeof(text_box_t) + textLen > BLE_RX_BUF_SIZE)
            {
                _busy = false;
                setStatus(STATUS_ERROR);
                return;
            }

            memcpy(_rxText, _rxBuf + sizeof(widget_t) + sizeof(text_box_t), textLen);
            _rxText[textLen] = '\0';

            std::shared_ptr<Widget> tb = std::make_shared<TextBox>(_rxText, &_rxHeader, &_rxTextBox);
            g_widgetManager->addWidget(std::move(tb));

            Serial.print("TextBox created: ");
            Serial.println(_rxText);
        }
    }
    else
    {
        Serial.print("Unknown transfer type: 0x");
        Serial.println((uint8_t)_rxType, HEX);
        _busy = false;
        setStatus(STATUS_ERROR);
        return;
    }

    _rxReceivedChunks++;

    if ((flags & 0x01) || _rxReceivedChunks >= _rxTotalChunks)
    {
        Serial.printf("Received: %u bytes written\n", (unsigned)_rxOffset);

        _busy = false;
        if (g_widgetManager)
        {
            g_widgetManager->draw();
        }
        setStatus(STATUS_SUCCESS);

        Serial.println("Transfer complete");
    }
}

void BLEManager::sendDataChunk(uint8_t type, uint8_t chunk_num, uint16_t total_chunks,
                               const uint8_t* data, uint16_t data_len, bool is_last)
{
    BleDataChunk chunk{};
    const uint16_t len = data_len > CHUNK_PAYLOAD_MAX ? CHUNK_PAYLOAD_MAX : data_len;

    chunk.type = type;
    chunk.chunk_num = chunk_num;
    chunk.total_chunks = (total_chunks >> 8) | (total_chunks << 8);
    chunk.flags = is_last ? 0x01 : 0x00;
    chunk.flags |= 0x02;

    const uint16_t crc = calculateCRC16(data, len);
    chunk.crc = (crc >> 8) | (crc << 8);

    memcpy(chunk.data, data, len);

    _charData.notify((uint8_t*)&chunk, CHUNK_HEADER_SIZE + len);

    delay(10);
}

uint16_t BLEManager::calculateCRC16(const uint8_t* data, uint16_t len)
{
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (int j = 0; j < 8; j++)
        {
            if (crc & 1) crc = (crc >> 1) ^ 0xA001;
            else crc >>= 1;
        }
    }

    return crc;
}

void BLEManager::setStatus(uint8_t status)
{
    _charStatus.write8(status);
    _charStatus.notify8(status);

    const char* status_str[] = {"IDLE", "BUSY", "SUCCESS", "ERROR", "WAITING", "SENDING"};
    if (status < 6)
    {
        Serial.print("Status: ");
        Serial.println(status_str[status]);
    }
}

bool BLEManager::isConnected()
{
    return Bluefruit.connected();
}

bool BLEManager::isBusy()
{
    return isConnected() || _busy;
}

void BLEManager::connect_callback(uint16_t conn_handle)
{
    BLEConnection* conn = Bluefruit.Connection(conn_handle);
    ble_gap_addr_t peer = conn->getPeerAddr();

    char dev_name[32] = {0};
    conn->getPeerName(dev_name, sizeof(dev_name));

    Serial.print("BLE Connected: ");
    Serial.print(dev_name);
    Serial.print(" (");
    for (int i = 5; i >= 0; i--)
    {
        if (peer.addr[i] < 0x10) Serial.print("0");
        Serial.print(peer.addr[i], HEX);
        if (i > 0) Serial.print(":");
    }
    Serial.println(")");
}

void BLEManager::disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    if (g_bleManager)
    {
        g_bleManager->_busy = false;
    }

    Serial.print("BLE Disconnected (reason=0x");
    Serial.print(reason, HEX);
    Serial.println(")");
}

void BLEManager::secured_callback(uint16_t conn_handle)
{
    Serial.println("BLE Secured");
}

bool BLEManager::pairing_passkey_callback(uint16_t conn_hdl, uint8_t const passkey[6], bool match_request)
{
    Serial.print("Pairing passkey: ");
    for (int i = 0; i < 6; i++) Serial.print(passkey[i]);
    Serial.println();
    Serial.flush();

    if (!match_request) return false;

    uint32_t t0 = millis();
    while (tick_diff(millis(), t0) <= PAIRING_TIMEOUT)
    {
        button.tick();
        if (button.isDoubleClick())
        {
            Serial.flush();
            return true;
        }
    }

    Serial.println("Pairing timeout");
    Serial.flush();
    return false;
}

void BLEManager::pairing_complete_callback(uint16_t conn_hdl, uint8_t auth_status)
{
    char dev_name[32] = {0};
    Bluefruit.Connection(conn_hdl)->getPeerName(dev_name, sizeof(dev_name));

    Serial.print("Paired: ");
    Serial.println(dev_name);
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
}
