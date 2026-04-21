#include "fantome/Modulation.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace fantome {

namespace {

constexpr float kPi = 3.14159265358979323846f;
constexpr std::array<float, 5> kQuarterNoteSpans {4.0f, 2.0f, 1.0f, 0.5f, 0.25f};

float ClampSampleRate(float sample_rate)
{
  return std::clamp(sample_rate, 8000.0f, 192000.0f);
}

float ClockRateHz(int division_index, float tempo_bpm)
{
  const auto clamped_index = std::clamp(division_index, 0, static_cast<int>(kQuarterNoteSpans.size() - 1));
  const auto quarter_note_span = kQuarterNoteSpans[static_cast<std::size_t>(clamped_index)];
  return (std::max(tempo_bpm, 1.0f) / 60.0f) / quarter_note_span;
}

}  // namespace

void LfoSource::SetSampleRate(float sample_rate)
{
  sample_rate_ = ClampSampleRate(sample_rate);
}

void LfoSource::Reset()
{
  phase_ = 0.0f;
  noise_state_ = 0x2468ace1u;
}

float LfoSource::Render(const ModulationSettings& settings, float tempo_bpm)
{
  const auto output = ShapeSample(settings.waveform, phase_);
  const auto rate_hz = EffectiveRateHz(settings, tempo_bpm);
  phase_ += rate_hz / sample_rate_;
  if (phase_ >= 1.0f) {
    phase_ -= std::floor(phase_);
  }
  return output;
}

float LfoSource::EffectiveRateHz(const ModulationSettings& settings, float tempo_bpm) const
{
  if (settings.sync_mode == SyncMode::MidiClock) {
    return ClockRateHz(settings.division_index, tempo_bpm);
  }
  return std::max(settings.rate_hz, 0.01f);
}

float LfoSource::ShapeSample(Waveform waveform, float phase)
{
  switch (waveform) {
    case Waveform::Sine:
      return std::sin(2.0f * kPi * phase);
    case Waveform::Triangle:
      return 1.0f - 4.0f * std::fabs(phase - 0.5f);
    case Waveform::Square:
      return phase < 0.5f ? 1.0f : -1.0f;
    case Waveform::Saw:
      return (2.0f * phase) - 1.0f;
    case Waveform::Noise:
      noise_state_ ^= noise_state_ << 13;
      noise_state_ ^= noise_state_ >> 17;
      noise_state_ ^= noise_state_ << 5;
      return (static_cast<float>(noise_state_ & 0x7fffffff) / 1073741824.0f) - 1.0f;
  }

  return 0.0f;
}

void SampleAndHoldSource::SetSampleRate(float sample_rate)
{
  sample_rate_ = ClampSampleRate(sample_rate);
}

void SampleAndHoldSource::Reset()
{
  phase_ = 0.0f;
  held_value_ = 0.0f;
  noise_state_ = 0x13579bdfu;
}

float SampleAndHoldSource::Render(const ModulationSettings& settings, float tempo_bpm)
{
  const auto rate_hz = EffectiveRateHz(settings, tempo_bpm);
  phase_ += rate_hz / sample_rate_;
  if (phase_ >= 1.0f) {
    phase_ -= std::floor(phase_);
    held_value_ = NextRandomValue();
  }
  return held_value_;
}

float SampleAndHoldSource::EffectiveRateHz(const ModulationSettings& settings, float tempo_bpm) const
{
  if (settings.sync_mode == SyncMode::MidiClock) {
    return ClockRateHz(settings.division_index, tempo_bpm);
  }
  return std::max(settings.rate_hz, 0.01f);
}

float SampleAndHoldSource::NextRandomValue()
{
  noise_state_ ^= noise_state_ << 13;
  noise_state_ ^= noise_state_ >> 17;
  noise_state_ ^= noise_state_ << 5;
  return (static_cast<float>(noise_state_ & 0x7fffffff) / 1073741824.0f) - 1.0f;
}

}  // namespace fantome

