#include "fantome/SynthVoice.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace fantome {

namespace {

constexpr float kPi = 3.14159265358979323846f;
constexpr std::array<float, kVoiceCount> kUnisonDetuneCents {-11.0f, -3.5f, 3.5f, 11.0f};
constexpr std::array<float, kVoiceCount> kVoicePhaseOffsets {0.0f, 0.19f, 0.41f, 0.67f};

float Clamp01(float value)
{
  return std::clamp(value, 0.0f, 1.0f);
}

float MidiNoteToHz(float note)
{
  return 440.0f * std::pow(2.0f, (note - 69.0f) / 12.0f);
}

float MapCutoffNormToHz(float normalized, float sample_rate)
{
  constexpr float kMinCutoff = 20.0f;
  const auto max_cutoff = sample_rate * 0.45f;
  const auto clamped = Clamp01(normalized);
  return kMinCutoff * std::pow(max_cutoff / kMinCutoff, clamped);
}

float TimeCoefficient(float time_s, float sample_rate)
{
  const auto safe_time = std::max(time_s, 0.0005f);
  return 1.0f - std::exp(-1.0f / (safe_time * sample_rate));
}

float PolyBlep(float phase, float phase_increment)
{
  if (phase_increment <= 0.0f || phase_increment >= 1.0f) {
    return 0.0f;
  }

  if (phase < phase_increment) {
    const auto t = phase / phase_increment;
    return t + t - t * t - 1.0f;
  }

  if (phase > 1.0f - phase_increment) {
    const auto t = (phase - 1.0f) / phase_increment;
    return t * t + t + t + 1.0f;
  }

  return 0.0f;
}

}  // namespace

void SynthVoice::SetSampleRate(float sample_rate)
{
  sample_rate_ = std::clamp(sample_rate, 8000.0f, 192000.0f);
}

void SynthVoice::SetVoiceIndex(std::size_t voice_index)
{
  voice_index_ = voice_index % kVoiceCount;
}

void SynthVoice::Reset()
{
  active_ = false;
  gate_ = false;
  note_ = 0;
  velocity_ = 0;
  velocity_gain_ = 0.0f;
  osc_a_phase_ = 0.0f;
  osc_b_phase_ = 0.0f;
  amp_stage_ = EnvelopeStage::Idle;
  amp_env_ = 0.0f;
  filter_stage_ = EnvelopeStage::Idle;
  filter_env_ = 0.0f;
  filter_low_ = 0.0f;
  filter_band_ = 0.0f;
  noise_state_ = 0x12345678u;
}

void SynthVoice::Start(std::uint8_t note, std::uint8_t velocity, bool retrigger)
{
  note_ = note;
  velocity_ = velocity;
  velocity_gain_ = std::clamp(static_cast<float>(velocity_) / 127.0f, 0.0f, 1.0f);
  active_ = true;
  gate_ = true;

  if (retrigger) {
    osc_a_phase_ = kVoicePhaseOffsets[voice_index_];
    osc_b_phase_ = std::fmod(kVoicePhaseOffsets[voice_index_] + 0.31f, 1.0f);
    amp_env_ = 0.0f;
    filter_env_ = 0.0f;
    amp_stage_ = EnvelopeStage::Attack;
    filter_stage_ = EnvelopeStage::Attack;
    filter_low_ *= 0.25f;
    filter_band_ *= 0.25f;
    return;
  }

  if (amp_stage_ == EnvelopeStage::Idle || amp_stage_ == EnvelopeStage::Release) {
    amp_stage_ = EnvelopeStage::Attack;
  }

  if (filter_stage_ == EnvelopeStage::Idle || filter_stage_ == EnvelopeStage::Release) {
    filter_stage_ = EnvelopeStage::Attack;
  }
}

void SynthVoice::Release()
{
  gate_ = false;
  if (amp_stage_ != EnvelopeStage::Idle) {
    amp_stage_ = EnvelopeStage::Release;
  }
  if (filter_stage_ != EnvelopeStage::Idle) {
    filter_stage_ = EnvelopeStage::Release;
  }
}

float SynthVoice::Render(
  const Patch& patch,
  const ModulationFrame& modulation,
  float pitch_bend)
{
  if (!active_) {
    return 0.0f;
  }

  const auto voice_detune_cents = VoiceDetuneCents(patch);
  const auto osc_a_hz = OscillatorFrequencyHz(
    patch.osc_a,
    pitch_bend,
    modulation.osc_pitch_semitones,
    patch.pitch_bend_range_semitones,
    voice_detune_cents);
  const auto osc_b_hz = OscillatorFrequencyHz(
    patch.osc_b,
    pitch_bend,
    modulation.osc_pitch_semitones,
    patch.pitch_bend_range_semitones,
    voice_detune_cents);

  const auto osc_a_sample = ProcessOscillator(
    patch.osc_a.waveform,
    osc_a_hz,
    patch.osc_a.pwm,
    osc_a_phase_);
  if (patch.osc_a.sync_enabled || patch.osc_b.sync_enabled) {
    if (osc_a_sample.wrapped) {
      osc_b_phase_ = 0.0f;
    }
  }

  const auto osc_b_sample = ProcessOscillator(
    patch.osc_b.waveform,
    osc_b_hz,
    patch.osc_b.pwm,
    osc_b_phase_);

  noise_state_ ^= noise_state_ << 13;
  noise_state_ ^= noise_state_ >> 17;
  noise_state_ ^= noise_state_ << 5;
  const auto noise = (static_cast<float>(noise_state_ & 0x7fffffff) / 1073741824.0f) - 1.0f;

  const auto amp_envelope = AdvanceAmpEnvelope(patch.amp_env);
  const auto filter_envelope = AdvanceFilterEnvelope(patch.filter_env);

  const auto total_source_level =
    std::max(1.0f, patch.osc_a.level + patch.osc_b.level + patch.noise_level);
  auto mixed = (osc_a_sample.value * patch.osc_a.level) +
               (osc_b_sample.value * patch.osc_b.level) +
               (noise * patch.noise_level);
  mixed *= 1.0f / std::sqrt(total_source_level);
  mixed = std::tanh(mixed * 1.18f) * 0.82f;

  auto cutoff_norm = patch.filter.cutoff;
  cutoff_norm += filter_envelope * patch.filter.env_amount * 0.55f;
  cutoff_norm += modulation.filter_cutoff_delta;
  cutoff_norm = Clamp01(cutoff_norm);

  const auto filtered = ProcessFilter(
    mixed,
    MapCutoffNormToHz(cutoff_norm, sample_rate_),
    patch.filter.resonance);

  const auto output =
    std::tanh(filtered * (1.08f + (patch.filter.resonance * 0.12f))) *
    amp_envelope *
    velocity_gain_;

  if (amp_stage_ == EnvelopeStage::Idle && !gate_) {
    active_ = false;
  }

  return output;
}

bool SynthVoice::IsActive() const
{
  return active_;
}

SynthVoice::OscillatorSample SynthVoice::ProcessOscillator(
  Waveform waveform,
  float frequency_hz,
  float pulse_width,
  float& phase)
{
  const auto clamped_frequency = std::clamp(frequency_hz, 1.0f, sample_rate_ * 0.45f);
  const auto phase_increment = clamped_frequency / sample_rate_;
  OscillatorSample result;

  switch (waveform) {
    case Waveform::Sine:
      result.value = std::sin(2.0f * kPi * phase);
      break;
    case Waveform::Triangle:
      result.value = 1.0f - 4.0f * std::fabs(phase - 0.5f);
      break;
    case Waveform::Square: {
      const auto width = std::clamp(pulse_width, 0.05f, 0.95f);
      result.value = phase < width ? 1.0f : -1.0f;
      result.value += PolyBlep(phase, phase_increment);
      const auto trailing_phase = std::fmod(phase - width + 1.0f, 1.0f);
      result.value -= PolyBlep(trailing_phase, phase_increment);
      break;
    }
    case Waveform::Saw:
      result.value = (2.0f * phase) - 1.0f;
      result.value -= PolyBlep(phase, phase_increment);
      break;
    case Waveform::Noise:
      noise_state_ ^= noise_state_ << 13;
      noise_state_ ^= noise_state_ >> 17;
      noise_state_ ^= noise_state_ << 5;
      result.value = (static_cast<float>(noise_state_ & 0x7fffffff) / 1073741824.0f) - 1.0f;
      break;
  }

  phase += phase_increment;
  if (phase >= 1.0f) {
    phase -= 1.0f;
    result.wrapped = true;
  }

  return result;
}

float SynthVoice::AdvanceAmpEnvelope(const AmpEnvelopeSettings& settings)
{
  const auto sustain = Clamp01(settings.sustain);

  switch (amp_stage_) {
    case EnvelopeStage::Idle:
      amp_env_ = 0.0f;
      break;
    case EnvelopeStage::Attack:
      amp_env_ += (1.0f - amp_env_) * TimeCoefficient(settings.attack_s, sample_rate_);
      if (amp_env_ >= 0.9995f) {
        amp_env_ = 1.0f;
        amp_stage_ = EnvelopeStage::Decay;
      }
      break;
    case EnvelopeStage::Decay:
      amp_env_ += (sustain - amp_env_) * TimeCoefficient(settings.decay_s, sample_rate_);
      if (std::fabs(amp_env_ - sustain) <= 0.0015f) {
        amp_env_ = sustain;
        amp_stage_ = gate_ ? EnvelopeStage::Sustain : EnvelopeStage::Release;
      }
      break;
    case EnvelopeStage::Sustain:
      amp_env_ = sustain;
      if (!gate_) {
        amp_stage_ = EnvelopeStage::Release;
      }
      break;
    case EnvelopeStage::Hold:
      break;
    case EnvelopeStage::Release:
      amp_env_ += (0.0f - amp_env_) * TimeCoefficient(settings.release_s, sample_rate_);
      if (amp_env_ <= 0.0001f) {
        amp_env_ = 0.0f;
        amp_stage_ = EnvelopeStage::Idle;
      }
      break;
  }

  return amp_env_;
}

float SynthVoice::AdvanceFilterEnvelope(const FilterEnvelopeSettings& settings)
{
  switch (filter_stage_) {
    case EnvelopeStage::Idle:
      filter_env_ = 0.0f;
      break;
    case EnvelopeStage::Attack:
      filter_env_ += (1.0f - filter_env_) * TimeCoefficient(settings.attack_s, sample_rate_);
      if (filter_env_ >= 0.9995f) {
        filter_env_ = 1.0f;
        filter_stage_ = gate_ ? EnvelopeStage::Hold : EnvelopeStage::Release;
      }
      break;
    case EnvelopeStage::Decay:
    case EnvelopeStage::Sustain:
      break;
    case EnvelopeStage::Hold:
      if (!gate_) {
        filter_stage_ = EnvelopeStage::Release;
      }
      break;
    case EnvelopeStage::Release:
      filter_env_ += (0.0f - filter_env_) * TimeCoefficient(settings.release_s, sample_rate_);
      if (filter_env_ <= 0.0001f) {
        filter_env_ = 0.0f;
        filter_stage_ = EnvelopeStage::Idle;
      }
      break;
  }

  return filter_env_;
}

float SynthVoice::ProcessFilter(float input, float cutoff_hz, float resonance)
{
  const auto cutoff = std::clamp(cutoff_hz, 20.0f, sample_rate_ * 0.45f);
  const auto frequency = std::clamp(
    2.0f * std::sin(kPi * cutoff / sample_rate_),
    0.001f,
    0.99f);
  const auto damping = std::clamp(1.98f - (resonance * 1.72f), 0.12f, 2.0f);
  const auto feedback = filter_band_ * (0.18f + (resonance * 0.72f));
  const auto driven_input = std::tanh((input - feedback) * (1.15f + (resonance * 0.55f)));
  filter_low_ += frequency * filter_band_;
  const auto high = driven_input - filter_low_ - (damping * filter_band_);
  filter_band_ += frequency * high;

  return filter_low_ * (1.0f + (resonance * 0.10f));
}

float SynthVoice::VoiceDetuneCents(const Patch& patch) const
{
  if (patch.play_mode != PlayMode::Unison) {
    return 0.0f;
  }

  return kUnisonDetuneCents[voice_index_];
}

float SynthVoice::OscillatorFrequencyHz(
  const OscillatorSettings& settings,
  float pitch_bend,
  float vibrato_semitones,
  int pitch_bend_range_semitones,
  float voice_detune_cents) const
{
  const auto pitch_bend_semitones =
    pitch_bend * static_cast<float>(std::max(pitch_bend_range_semitones, 0));
  const auto note_semitones =
    static_cast<float>(note_) +
    static_cast<float>(settings.octave * 12) +
    (settings.fine_tune_cents / 100.0f) +
    (voice_detune_cents / 100.0f) +
    pitch_bend_semitones +
    vibrato_semitones;

  return MidiNoteToHz(note_semitones);
}

}  // namespace fantome
