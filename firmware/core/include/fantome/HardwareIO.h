#pragma once

#include <array>
#include <cstddef>
#include <string>

#include "fantome/FantomeEngine.h"
#include "fantome/OledView.h"
#include "fantome/PortableInput.h"
#include "fantome/UiState.h"

namespace fantome {

enum class HardwareButtonId {
  Encoder = 0,
  PagePrevious,
  PageNext,
  Back,
  Action,
  Count,
};

struct HardwareButtonState {
  bool pressed = false;
  bool just_pressed = false;
};

struct HardwarePotState {
  bool available = false;
  float normalized = 0.0f;
};

struct HardwareInputFrame {
  static constexpr std::size_t kMidiCapacity = 16;

  int encoder_delta = 0;
  std::array<HardwareButtonState, static_cast<std::size_t>(HardwareButtonId::Count)> buttons {};
  std::array<HardwarePotState, 8> pots {};
  std::array<MidiMessage, kMidiCapacity> midi_messages {};
  std::size_t midi_message_count = 0;

  bool PushMidi(const MidiMessage& message);
  void ClearTransient();
};

struct HardwareOutputFrame {
  OledTextFrame oled {};
  bool midi_activity = false;
  bool preset_dirty = false;
  bool session_dirty = false;
  bool startup_active = false;
  std::size_t active_preset_slot = 0;
  std::string session_summary;
};

class HardwareControlRouter {
 public:
  void Reset();
  bool Apply(const HardwareInputFrame& frame, FantomeEngine& engine, UiState& ui);

  const PortableInputSurface& InputSurface() const;

 private:
  PortableInputSurface input_surface_ {};
};

}  // namespace fantome
