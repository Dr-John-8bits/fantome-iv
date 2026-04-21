#pragma once

#include <array>
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
  patch.osc_a.waveform = Waveform::Saw;
  patch.osc_b.waveform = Waveform::Square;
  patch.osc_b.level = 0.65f;
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

inline std::array<Patch, kPresetCount> MakeFactoryPresetBank()
{
  auto bank = std::array<Patch, kPresetCount> {
    MakeInitPatch("Preset 1"),
    MakeInitPatch("Preset 2"),
    MakeInitPatch("Preset 3"),
    MakeInitPatch("Preset 4"),
  };

  bank[0].name = "Ghost Pad";
  bank[0].osc_a.waveform = Waveform::Saw;
  bank[0].osc_b.waveform = Waveform::Triangle;
  bank[0].filter.cutoff = 0.58f;
  bank[0].amp_env.attack_s = 0.45f;
  bank[0].amp_env.release_s = 1.20f;
  bank[0].osc_lfo.amount = 0.08f;
  bank[0].osc_lfo.rate_hz = 0.32f;
  bank[0].filter_lfo.amount = 0.10f;
  bank[0].filter_lfo.rate_hz = 0.18f;
  bank[0].chorus.depth = 0.52f;
  bank[0].reverb.mix = 0.28f;

  bank[1].name = "Glass Keys";
  bank[1].osc_a.waveform = Waveform::Triangle;
  bank[1].osc_b.waveform = Waveform::Square;
  bank[1].filter.cutoff = 0.68f;
  bank[1].filter.resonance = 0.24f;
  bank[1].filter_sample_hold.amount = 0.08f;
  bank[1].filter_sample_hold.sync_mode = SyncMode::MidiClock;
  bank[1].filter_sample_hold.division_index = 3;
  bank[1].delay.mix = 0.18f;
  bank[1].delay.feedback = 0.26f;

  bank[2].name = "Phantom Brass";
  bank[2].osc_a.waveform = Waveform::Saw;
  bank[2].osc_b.waveform = Waveform::Saw;
  bank[2].osc_b.octave = -1;
  bank[2].filter.cutoff = 0.48f;
  bank[2].filter.env_amount = 0.62f;
  bank[2].filter_lfo.amount = 0.06f;
  bank[2].filter_lfo.rate_hz = 4.8f;
  bank[2].amp_env.attack_s = 0.04f;
  bank[2].amp_env.release_s = 0.18f;

  bank[3].name = "Init Lead";
  bank[3].play_mode = PlayMode::Mono;
  bank[3].osc_a.waveform = Waveform::Square;
  bank[3].osc_b.waveform = Waveform::Saw;
  bank[3].osc_b.fine_tune_cents = 6.0f;
  bank[3].filter.cutoff = 0.74f;
  bank[3].osc_lfo.amount = 0.12f;
  bank[3].osc_lfo.rate_hz = 5.2f;
  bank[3].delay.mix = 0.12f;

  return bank;
}

}  // namespace fantome
