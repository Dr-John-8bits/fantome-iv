#pragma once

#include <cstddef>
#include <cstdint>

#include "fantome/Modulation.h"
#include "fantome/Patch.h"

namespace fantome {

class SynthVoice {
 public:
  void SetSampleRate(float sample_rate);
  void SetVoiceIndex(std::size_t voice_index);
  void Reset();
  void Start(std::uint8_t note, std::uint8_t velocity, bool retrigger);
  void Release();
  float Render(const Patch& patch, const ModulationFrame& modulation, float pitch_bend);
  bool IsActive() const;

 private:
  enum class EnvelopeStage {
    Idle,
    Attack,
    Decay,
    Sustain,
    Hold,
    Release,
  };

  struct OscillatorSample {
    float value = 0.0f;
    bool wrapped = false;
  };

  OscillatorSample ProcessOscillator(
    Waveform waveform,
    float frequency_hz,
    float pulse_width,
    float& phase);
  float AdvanceAmpEnvelope(const AmpEnvelopeSettings& settings);
  float AdvanceFilterEnvelope(const FilterEnvelopeSettings& settings);
  float ProcessFilter(float input, float cutoff_hz, float resonance);
  float VoiceDetuneCents(const Patch& patch) const;
  float OscillatorFrequencyHz(
    const OscillatorSettings& settings,
    float pitch_bend,
    float vibrato_semitones,
    int pitch_bend_range_semitones,
    float voice_detune_cents) const;

  float sample_rate_ = 48000.0f;
  std::size_t voice_index_ = 0;
  bool active_ = false;
  bool gate_ = false;
  std::uint8_t note_ = 0;
  std::uint8_t velocity_ = 0;
  float velocity_gain_ = 0.0f;

  float osc_a_phase_ = 0.0f;
  float osc_b_phase_ = 0.0f;

  EnvelopeStage amp_stage_ = EnvelopeStage::Idle;
  float amp_env_ = 0.0f;

  EnvelopeStage filter_stage_ = EnvelopeStage::Idle;
  float filter_env_ = 0.0f;

  float filter_low_ = 0.0f;
  float filter_band_ = 0.0f;

  std::uint32_t noise_state_ = 0x12345678u;
};

}  // namespace fantome
