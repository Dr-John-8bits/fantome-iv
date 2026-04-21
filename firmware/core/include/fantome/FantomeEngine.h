#pragma once

#include <array>
#include <cstddef>

#include "fantome/MidiTypes.h"
#include "fantome/Patch.h"
#include "fantome/VoiceAllocator.h"

namespace fantome {

struct TransportState {
  bool running = false;
  float tempo_bpm = 120.0f;
  std::uint64_t midi_clock_pulses = 0;
};

struct PerformanceState {
  float mod_wheel = 0.0f;
  float pitch_bend = 0.0f;
  bool sustain = false;
};

class FantomeEngine {
 public:
  FantomeEngine();

  void Reset();
  void HandleMidi(const MidiMessage& message);
  void SetClockTempoBpm(float bpm);
  void SavePreset(std::size_t slot);
  bool LoadPreset(std::size_t slot);

  Patch& CurrentPatchMutable();
  const Patch& CurrentPatch() const;
  const std::array<Patch, kPresetCount>& PresetBank() const;
  const std::array<VoiceState, kVoiceCount>& Voices() const;
  const TransportState& Transport() const;
  const PerformanceState& Performance() const;

 private:
  bool MatchesCurrentChannel(const MidiMessage& message) const;
  void HandleNoteOn(std::uint8_t note, std::uint8_t velocity);
  void HandleNoteOff(std::uint8_t note);
  void HandleControlChange(std::uint8_t controller, std::uint8_t value);
  void HandleProgramChange(std::uint8_t program);
  void HandlePitchBend(int value);
  static float NormalizeMidi7(std::uint8_t value);

  Patch patch_ {};
  std::array<Patch, kPresetCount> preset_bank_ {};
  VoiceAllocator allocator_ {};
  TransportState transport_ {};
  PerformanceState performance_ {};
};

}  // namespace fantome

