#pragma once

#include "fantome/Patch.h"

namespace fantome {

struct ModulationFrame {
  float osc_pitch_semitones = 0.0f;
  float filter_cutoff_delta = 0.0f;
};

class LfoSource {
 public:
  void SetSampleRate(float sample_rate);
  void Reset();
  float Render(const ModulationSettings& settings, float tempo_bpm);

 private:
  float EffectiveRateHz(const ModulationSettings& settings, float tempo_bpm) const;
  float ShapeSample(Waveform waveform, float phase);

  float sample_rate_ = 48000.0f;
  float phase_ = 0.0f;
  std::uint32_t noise_state_ = 0x2468ace1u;
};

class SampleAndHoldSource {
 public:
  void SetSampleRate(float sample_rate);
  void Reset();
  float Render(const ModulationSettings& settings, float tempo_bpm);

 private:
  float EffectiveRateHz(const ModulationSettings& settings, float tempo_bpm) const;
  float NextRandomValue();

  float sample_rate_ = 48000.0f;
  float phase_ = 0.0f;
  float held_value_ = 0.0f;
  std::uint32_t noise_state_ = 0x13579bdfu;
};

}  // namespace fantome

