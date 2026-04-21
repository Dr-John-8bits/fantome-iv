#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

#include "DaisyPeripherals.h"
#include "fantome/HardwareIO.h"

namespace fantome {

struct DaisyEncoderTargetConfig {
  int a_board_pin = 1;
  const char* a_pin_label = "D0";
  const char* a_gpio = "PB12";
  int b_board_pin = 2;
  const char* b_pin_label = "D1";
  const char* b_gpio = "PC11";
  int sw_board_pin = 3;
  const char* sw_pin_label = "D2";
  const char* sw_gpio = "PC10";
  bool switch_active_low = true;
  bool switch_pullup = true;
};

struct DaisyButtonTargetConfig {
  HardwareButtonId id = HardwareButtonId::PagePrevious;
  const char* logical_name = "";
  int board_pin = 0;
  const char* pin_label = "";
  const char* gpio = "";
  bool active_low = true;
  bool pullup = true;
};

struct DaisyLedTargetConfig {
  const char* logical_name = "LED_MIDI";
  int board_pin = 8;
  const char* pin_label = "D7";
  const char* gpio = "PG10";
  bool active_high = true;
};

struct DaisyPotTargetConfig {
  std::size_t pot_index = 0;
  const char* logical_name = "";
  int board_pin = 0;
  const char* pin_label = "";
  const char* gpio = "";
};

struct DaisyOledTargetConfig {
  const char* controller = "SH1106";
  std::size_t width = 128;
  std::size_t height = 64;
  std::uint8_t i2c_address = 0x3C;
  int scl_board_pin = 12;
  const char* scl_pin_label = "D11";
  const char* scl_gpio = "PB8";
  int sda_board_pin = 13;
  const char* sda_pin_label = "D12";
  const char* sda_gpio = "PB9";
};

struct DaisyMidiTargetConfig {
  std::uint32_t baud = 31250;
  int rx_board_pin = 15;
  const char* rx_pin_label = "D14";
  const char* rx_gpio = "PB7";
  int tx_board_pin = 14;
  const char* tx_pin_label = "D13";
  const char* tx_gpio = "PB6";
  bool opto_isolated = true;
};

struct DaisySessionTargetConfig {
  const char* backend_label = "file-stub";
  const char* boot_policy = "auto-restore";
  const char* relative_path = "runtime/fantome_iv_session.txt";
};

struct DaisyTargetConfig {
  DaisyAudioConfig audio {};
  DaisyEncoderTargetConfig encoder {};
  std::array<DaisyButtonTargetConfig, 4> buttons {};
  DaisyLedTargetConfig midi_led {};
  std::array<DaisyPotTargetConfig, 8> pots {};
  DaisyOledTargetConfig oled {};
  DaisyMidiTargetConfig midi {};
  DaisySessionTargetConfig session {};
};

const DaisyTargetConfig& DefaultDaisyTargetConfig();
std::string DaisyTargetConfigSummary(const DaisyTargetConfig& config);

}  // namespace fantome
