#pragma once

#include <Arduino.h>

namespace Proto {
  enum Cmd : uint8_t {
    kCmdSaveScreen = 0x10,
    kCmdLoadScreen = 0x11,
    kCmdClearWidgets = 0x12,
    kCmdDeleteWidget = 0x13,
    kCmdClearBackground = 0x22,
    kCmdRecvWidget = 0x31,
    kCmdRecvBackground = 0x33,
    kCmdRefreshDisplay = 0x40,
    kCmdLoadDefaultScreen = 0x52,
  };

  enum Status : uint8_t {
    kStatusIdle = 0x00, // not using last 5 seconds
    kStatusBusy = 0x01, // busy by another device
    kStatusSuccess = 0x02,
    kStatusError = 0x03,
    kStatusWaiting = 0x04,
    kStatusSending = 0x05,
  };
#ifndef ARDUINO
  struct Entry {
    Cmd id;
    const char *name;
  };

  constexpr Entry kStatus[] = {
      {CmdSaveScreen, "Save screen"},
      {CmdLoadScreen, "Load screen"},
      {CmdClearWidgets, "Clear widgets"},
      {CmdDeleteWidget, "Delete widget"},
      {CmdClearBackground, "Clear background"},
      {CmdRecvWidget, "Recv widget"},
      {CmdRecvBackground, "Recv background"},
      {CmdRefreshDisplay, "Refresh display"},
      {CmdLoadDefaultScreen, "Load default screen"},

  };
#endif

  struct DataChunk {
    uint8_t type;
    uint8_t chunk_num;
    uint16_t total_chunks;
    uint8_t flags;
    uint16_t crc;
    uint8_t data[248];
  } __attribute__((packed));

  // WidgetTypes type;
  // WidgetTypes
  enum WidgetTypes : uint8_t {
    kTypeNone = 0x00,
    kTypeTextBox = 0x01,
    kTypeBackground = 0xFF,
  };

  constexpr uint8_t kAppendIndex = 0xFF;

  // Largest payload that fits into one ATT write: MTU 247 - 3 (ATT) - 7 (chunk
  // header). Do NOT derive this from the MTU reported by the BLE stack at connect
  // time: many stacks still report the default 23 before the exchange finishes.
  constexpr int kChunkPayload = 237;
  constexpr int kMinChunkPayload = 20;
  constexpr int kChunkHeader = 7;

  // chunk_num in the header is a single byte
  constexpr int kMaxChunks = 255;
  constexpr int kBackgroundBytes = 30000;

} // namespace Proto
