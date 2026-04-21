#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

#include "fantome/Effects.h"
#include "fantome/Modulation.h"
#include "fantome/MidiTypes.h"
#include "fantome/Patch.h"
#include "fantome/SynthVoice.h"
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

struct EngineSessionState {
  Patch current_patch {};
  std::array<Patch, kPresetCount> preset_bank {};
  std::size_t active_preset_slot = 0;
  float tempo_bpm = 120.0f;
};

class FantomeEngine {
 public:
  FantomeEngine();

  void Reset();
  void SetSampleRate(float sample_rate);
  void HandleMidi(const MidiMessage& message);
  void SetClockTempoBpm(float bpm);
  void SavePreset(std::size_t slot);
  bool LoadPreset(std::size_t slot);
  void InitializeCurrentPatch();
  bool SavePresetBankToFile(const std::string& path) const;
  bool LoadPresetBankFromFile(const std::string& path);
  void Render(float* left, float* right, std::size_t frame_count);

  Patch& CurrentPatchMutable();
  const Patch& CurrentPatch() const;
  const std::array<Patch, kPresetCount>& PresetBank() const;
  const std::array<VoiceState, kVoiceCount>& Voices() const;
  const TransportState& Transport() const;
  const PerformanceState& Performance() const;
  const ModulationFrame& LastModulationFrame() const;
  float SampleRate() const;
  std::size_t CurrentPresetSlot() const;
  bool IsCurrentPresetDirty() const;
  EngineSessionState ExportSessionState() const;
  bool RestoreSessionState(const EngineSessionState& state);

 private:
  bool MatchesCurrentChannel(const MidiMessage& message) const;
  void HandleNoteOn(std::uint8_t note, std::uint8_t velocity);
  void HandleNoteOff(std::uint8_t note);
  void HandleControlChange(std::uint8_t controller, std::uint8_t value);
  void HandleProgramChange(std::uint8_t program);
  void HandlePitchBend(int value);
  void SyncVoicesFromAllocator();
  void ResetDspVoices();
  ModulationFrame BuildModulationFrame();
  float ProcessDcBlock(float input, float& input_state, float& output_state);
  static float NormalizeMidi7(std::uint8_t value);

  Patch patch_ {};
  std::array<Patch, kPresetCount> preset_bank_ {};
  VoiceAllocator allocator_ {};
  std::array<SynthVoice, kVoiceCount> dsp_voices_ {};
  std::array<VoiceState, kVoiceCount> allocator_snapshot_ {};
  StereoChorusEffect chorus_ {};
  StereoDelayEffect delay_ {};
  StereoReverbEffect reverb_ {};
  LfoSource osc_lfo_source_ {};
  LfoSource filter_lfo_source_ {};
  SampleAndHoldSource filter_sample_hold_source_ {};
  TransportState transport_ {};
  PerformanceState performance_ {};
  ModulationFrame last_modulation_ {};
  float sample_rate_ = 48000.0f;
  std::size_t active_preset_slot_ = 0;
  float output_dc_left_in_ = 0.0f;
  float output_dc_left_out_ = 0.0f;
  float output_dc_right_in_ = 0.0f;
  float output_dc_right_out_ = 0.0f;
};

}  // namespace fantome
