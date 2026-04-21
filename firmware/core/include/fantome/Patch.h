#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <string>

namespace fantome {

constexpr std::size_t kVoiceCount = 4;
constexpr std::size_t kPresetCount = 4;

enum class Waveform {
  Sine,
  Triangle,
  Square,
  Saw,
  Noise,
};

enum class PlayMode {
  Poly,
  Mono,
  Unison,
};

enum class SyncMode {
  Free,
  MidiClock,
};

struct OscillatorSettings {
  Waveform waveform = Waveform::Saw;
  int octave = 0;
  float fine_tune_cents = 0.0f;
  float level = 0.8f;
  float pwm = 0.5f;
  bool sync_enabled = false;
};

struct FilterSettings {
  float cutoff = 0.72f;
  float resonance = 0.18f;
  float env_amount = 0.45f;
  float lfo_amount = 0.0f;
  float sample_hold_amount = 0.0f;
};

struct AmpEnvelopeSettings {
  float attack_s = 0.01f;
  float decay_s = 0.20f;
  float sustain = 0.82f;
  float release_s = 0.35f;
};

struct FilterEnvelopeSettings {
  float attack_s = 0.01f;
  float release_s = 0.25f;
};

struct ModulationSettings {
  SyncMode sync_mode = SyncMode::Free;
  Waveform waveform = Waveform::Sine;
  float rate_hz = 2.0f;
  float amount = 0.0f;
  int division_index = 2;
};

struct ChorusSettings {
  float depth = 0.35f;
  float rate_hz = 0.35f;
  float mix = 0.18f;
};

struct DelaySettings {
  SyncMode sync_mode = SyncMode::MidiClock;
  float time_ms = 375.0f;
  int division_index = 2;
  float feedback = 0.32f;
  float mix = 0.24f;
};

struct ReverbSettings {
  float mix = 0.14f;
};

struct Patch {
  std::string name = "Init";
  PlayMode play_mode = PlayMode::Poly;
  bool legato_enabled = true;
  std::uint8_t midi_channel = 1;
  int pitch_bend_range_semitones = 2;
  float master_volume = 0.82f;
  OscillatorSettings osc_a {};
  OscillatorSettings osc_b {};
  float noise_level = 0.0f;
  FilterSettings filter {};
  AmpEnvelopeSettings amp_env {};
  FilterEnvelopeSettings filter_env {};
  ModulationSettings osc_lfo {};
  ModulationSettings filter_lfo {};
  ModulationSettings filter_sample_hold {
    SyncMode::MidiClock,
    Waveform::Noise,
    4.0f,
    0.0f,
    2,
  };
  ChorusSettings chorus {};
  DelaySettings delay {};
  ReverbSettings reverb {};
};

inline Patch MakeInitPatch(const std::string& name = "Init")
{
  Patch patch;
  patch.name = name;
  patch.master_volume = 0.78f;
  patch.osc_a.waveform = Waveform::Saw;
  patch.osc_b.waveform = Waveform::Square;
  patch.osc_b.level = 0.58f;
  patch.osc_b.fine_tune_cents = 4.0f;
  patch.filter.cutoff = 0.72f;
  patch.filter.resonance = 0.18f;
  patch.osc_lfo.rate_hz = 4.5f;
  patch.osc_lfo.amount = 0.0f;
  patch.filter_lfo.waveform = Waveform::Triangle;
  patch.filter_lfo.rate_hz = 0.8f;
  patch.filter_lfo.amount = 0.0f;
  patch.filter_sample_hold.sync_mode = SyncMode::MidiClock;
  patch.filter_sample_hold.division_index = 2;
  patch.filter_sample_hold.amount = 0.0f;
  patch.chorus.depth = 0.35f;
  patch.delay.feedback = 0.32f;
  patch.delay.mix = 0.24f;
  patch.reverb.mix = 0.14f;
  return patch;
}

inline std::array<Patch, kPresetCount> MakeDefaultUserPresetBank()
{
  auto bank = std::array<Patch, kPresetCount> {
    MakeInitPatch("Preset 1"),
    MakeInitPatch("Preset 2"),
    MakeInitPatch("Preset 3"),
    MakeInitPatch("Preset 4"),
  };

  bank[0].name = "Ghost Pad";
  bank[0].master_volume = 0.70f;
  bank[0].osc_a.waveform = Waveform::Saw;
  bank[0].osc_b.waveform = Waveform::Triangle;
  bank[0].osc_b.level = 0.42f;
  bank[0].osc_b.fine_tune_cents = 5.0f;
  bank[0].noise_level = 0.02f;
  bank[0].filter.cutoff = 0.52f;
  bank[0].filter.resonance = 0.18f;
  bank[0].filter.env_amount = 0.38f;
  bank[0].amp_env.attack_s = 0.62f;
  bank[0].amp_env.decay_s = 0.34f;
  bank[0].amp_env.sustain = 0.80f;
  bank[0].amp_env.release_s = 1.45f;
  bank[0].osc_lfo.amount = 0.05f;
  bank[0].osc_lfo.rate_hz = 0.24f;
  bank[0].filter_lfo.amount = 0.08f;
  bank[0].filter_lfo.rate_hz = 0.14f;
  bank[0].chorus.depth = 0.58f;
  bank[0].chorus.rate_hz = 0.28f;
  bank[0].chorus.mix = 0.34f;
  bank[0].delay.division_index = 3;
  bank[0].delay.mix = 0.16f;
  bank[0].delay.feedback = 0.26f;
  bank[0].reverb.mix = 0.22f;

  bank[1].name = "Glass Keys";
  bank[1].master_volume = 0.74f;
  bank[1].osc_a.waveform = Waveform::Triangle;
  bank[1].osc_b.waveform = Waveform::Sine;
  bank[1].osc_b.level = 0.28f;
  bank[1].osc_b.octave = 1;
  bank[1].osc_a.level = 0.72f;
  bank[1].noise_level = 0.01f;
  bank[1].filter.cutoff = 0.74f;
  bank[1].filter.resonance = 0.18f;
  bank[1].filter.env_amount = 0.28f;
  bank[1].amp_env.attack_s = 0.003f;
  bank[1].amp_env.decay_s = 0.19f;
  bank[1].amp_env.sustain = 0.12f;
  bank[1].amp_env.release_s = 0.20f;
  bank[1].filter_env.attack_s = 0.002f;
  bank[1].filter_env.release_s = 0.18f;
  bank[1].filter_sample_hold.amount = 0.05f;
  bank[1].filter_sample_hold.sync_mode = SyncMode::MidiClock;
  bank[1].filter_sample_hold.division_index = 3;
  bank[1].chorus.depth = 0.18f;
  bank[1].chorus.rate_hz = 0.42f;
  bank[1].chorus.mix = 0.12f;
  bank[1].delay.division_index = 3;
  bank[1].delay.mix = 0.12f;
  bank[1].delay.feedback = 0.18f;
  bank[1].reverb.mix = 0.08f;

  bank[2].name = "Phantom Brass";
  bank[2].master_volume = 0.62f;
  bank[2].play_mode = PlayMode::Unison;
  bank[2].legato_enabled = false;
  bank[2].osc_a.waveform = Waveform::Saw;
  bank[2].osc_b.waveform = Waveform::Saw;
  bank[2].osc_b.octave = 0;
  bank[2].osc_b.fine_tune_cents = 9.0f;
  bank[2].osc_b.level = 0.70f;
  bank[2].noise_level = 0.03f;
  bank[2].filter.cutoff = 0.42f;
  bank[2].filter.resonance = 0.30f;
  bank[2].filter.env_amount = 0.70f;
  bank[2].filter_lfo.amount = 0.04f;
  bank[2].filter_lfo.rate_hz = 5.4f;
  bank[2].amp_env.attack_s = 0.018f;
  bank[2].amp_env.decay_s = 0.16f;
  bank[2].amp_env.sustain = 0.70f;
  bank[2].amp_env.release_s = 0.12f;
  bank[2].chorus.depth = 0.20f;
  bank[2].chorus.mix = 0.08f;
  bank[2].delay.mix = 0.06f;
  bank[2].reverb.mix = 0.06f;

  bank[3].name = "Init Lead";
  bank[3].master_volume = 0.70f;
  bank[3].play_mode = PlayMode::Mono;
  bank[3].legato_enabled = true;
  bank[3].osc_a.waveform = Waveform::Square;
  bank[3].osc_b.waveform = Waveform::Saw;
  bank[3].osc_a.sync_enabled = true;
  bank[3].osc_b.sync_enabled = true;
  bank[3].osc_b.fine_tune_cents = 8.0f;
  bank[3].osc_b.level = 0.64f;
  bank[3].filter.cutoff = 0.70f;
  bank[3].filter.resonance = 0.24f;
  bank[3].filter.env_amount = 0.58f;
  bank[3].amp_env.attack_s = 0.003f;
  bank[3].amp_env.decay_s = 0.10f;
  bank[3].amp_env.sustain = 0.60f;
  bank[3].amp_env.release_s = 0.10f;
  bank[3].filter_env.attack_s = 0.002f;
  bank[3].filter_env.release_s = 0.14f;
  bank[3].osc_lfo.amount = 0.10f;
  bank[3].osc_lfo.rate_hz = 5.8f;
  bank[3].chorus.depth = 0.16f;
  bank[3].chorus.rate_hz = 0.52f;
  bank[3].chorus.mix = 0.10f;
  bank[3].delay.sync_mode = SyncMode::Free;
  bank[3].delay.time_ms = 145.0f;
  bank[3].delay.feedback = 0.18f;
  bank[3].delay.mix = 0.10f;
  bank[3].reverb.mix = 0.04f;

  return bank;
}

inline bool NearlyEqual(float left, float right, float epsilon = 0.0001f)
{
  return std::fabs(left - right) <= epsilon;
}

inline bool PatchApproximatelyEqual(const Patch& left, const Patch& right)
{
  return left.name == right.name &&
         left.play_mode == right.play_mode &&
         left.legato_enabled == right.legato_enabled &&
         left.midi_channel == right.midi_channel &&
         left.pitch_bend_range_semitones == right.pitch_bend_range_semitones &&
         NearlyEqual(left.master_volume, right.master_volume) &&
         left.osc_a.waveform == right.osc_a.waveform &&
         left.osc_a.octave == right.osc_a.octave &&
         NearlyEqual(left.osc_a.fine_tune_cents, right.osc_a.fine_tune_cents) &&
         NearlyEqual(left.osc_a.level, right.osc_a.level) &&
         NearlyEqual(left.osc_a.pwm, right.osc_a.pwm) &&
         left.osc_a.sync_enabled == right.osc_a.sync_enabled &&
         left.osc_b.waveform == right.osc_b.waveform &&
         left.osc_b.octave == right.osc_b.octave &&
         NearlyEqual(left.osc_b.fine_tune_cents, right.osc_b.fine_tune_cents) &&
         NearlyEqual(left.osc_b.level, right.osc_b.level) &&
         NearlyEqual(left.osc_b.pwm, right.osc_b.pwm) &&
         left.osc_b.sync_enabled == right.osc_b.sync_enabled &&
         NearlyEqual(left.noise_level, right.noise_level) &&
         NearlyEqual(left.filter.cutoff, right.filter.cutoff) &&
         NearlyEqual(left.filter.resonance, right.filter.resonance) &&
         NearlyEqual(left.filter.env_amount, right.filter.env_amount) &&
         NearlyEqual(left.filter.lfo_amount, right.filter.lfo_amount) &&
         NearlyEqual(left.filter.sample_hold_amount, right.filter.sample_hold_amount) &&
         NearlyEqual(left.amp_env.attack_s, right.amp_env.attack_s) &&
         NearlyEqual(left.amp_env.decay_s, right.amp_env.decay_s) &&
         NearlyEqual(left.amp_env.sustain, right.amp_env.sustain) &&
         NearlyEqual(left.amp_env.release_s, right.amp_env.release_s) &&
         NearlyEqual(left.filter_env.attack_s, right.filter_env.attack_s) &&
         NearlyEqual(left.filter_env.release_s, right.filter_env.release_s) &&
         left.osc_lfo.sync_mode == right.osc_lfo.sync_mode &&
         left.osc_lfo.waveform == right.osc_lfo.waveform &&
         NearlyEqual(left.osc_lfo.rate_hz, right.osc_lfo.rate_hz) &&
         NearlyEqual(left.osc_lfo.amount, right.osc_lfo.amount) &&
         left.osc_lfo.division_index == right.osc_lfo.division_index &&
         left.filter_lfo.sync_mode == right.filter_lfo.sync_mode &&
         left.filter_lfo.waveform == right.filter_lfo.waveform &&
         NearlyEqual(left.filter_lfo.rate_hz, right.filter_lfo.rate_hz) &&
         NearlyEqual(left.filter_lfo.amount, right.filter_lfo.amount) &&
         left.filter_lfo.division_index == right.filter_lfo.division_index &&
         left.filter_sample_hold.sync_mode == right.filter_sample_hold.sync_mode &&
         left.filter_sample_hold.waveform == right.filter_sample_hold.waveform &&
         NearlyEqual(left.filter_sample_hold.rate_hz, right.filter_sample_hold.rate_hz) &&
         NearlyEqual(left.filter_sample_hold.amount, right.filter_sample_hold.amount) &&
         left.filter_sample_hold.division_index == right.filter_sample_hold.division_index &&
         NearlyEqual(left.chorus.depth, right.chorus.depth) &&
         NearlyEqual(left.chorus.rate_hz, right.chorus.rate_hz) &&
         NearlyEqual(left.chorus.mix, right.chorus.mix) &&
         left.delay.sync_mode == right.delay.sync_mode &&
         NearlyEqual(left.delay.time_ms, right.delay.time_ms) &&
         left.delay.division_index == right.delay.division_index &&
         NearlyEqual(left.delay.feedback, right.delay.feedback) &&
         NearlyEqual(left.delay.mix, right.delay.mix) &&
         NearlyEqual(left.reverb.mix, right.reverb.mix);
}

}  // namespace fantome
