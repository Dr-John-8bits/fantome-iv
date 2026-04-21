#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "fantome/FantomeEngine.h"
#include "fantome/FirmwareRuntime.h"
#include "fantome/HardwareIO.h"
#include "fantome/OledView.h"
#include "fantome/PortableInput.h"
#include "fantome/SessionManager.h"
#include "fantome/SessionPersistence.h"
#include "fantome/StartupDisplay.h"
#include "fantome/UiState.h"
#include "DaisyApp.h"
#include "DaisyPlatformStub.h"

namespace {

void Expect(bool condition, const std::string& message)
{
  if (!condition) {
    throw std::runtime_error(message);
  }
}

bool HasActiveNote(const fantome::FantomeEngine& engine, std::uint8_t note)
{
  for (const auto& voice : engine.Voices()) {
    if (voice.active && voice.note == note) {
      return true;
    }
  }
  return false;
}

float BufferEnergy(const std::vector<float>& left, const std::vector<float>& right)
{
  float energy = 0.0f;
  for (std::size_t index = 0; index < left.size(); ++index) {
    energy += (left[index] * left[index]) + (right[index] * right[index]);
  }
  return energy;
}

float StereoDifference(const std::vector<float>& left, const std::vector<float>& right)
{
  float difference = 0.0f;
  for (std::size_t index = 0; index < left.size(); ++index) {
    difference += std::fabs(left[index] - right[index]);
  }
  return difference;
}

float MaxAbsSample(const std::vector<float>& left, const std::vector<float>& right)
{
  float max_value = 0.0f;
  for (std::size_t index = 0; index < left.size(); ++index) {
    max_value = std::max(max_value, std::fabs(left[index]));
    max_value = std::max(max_value, std::fabs(right[index]));
  }
  return max_value;
}

float RenderPlanEnergy(
  fantome::FantomeEngine engine,
  const std::vector<std::size_t>& block_plan)
{
  const auto max_block = *std::max_element(block_plan.begin(), block_plan.end());
  std::vector<float> left(max_block, 0.0f);
  std::vector<float> right(max_block, 0.0f);
  float total_energy = 0.0f;

  for (const auto frames : block_plan) {
    std::fill(left.begin(), left.end(), 0.0f);
    std::fill(right.begin(), right.end(), 0.0f);
    engine.Render(left.data(), right.data(), frames);

    for (std::size_t index = 0; index < frames; ++index) {
      total_energy += (left[index] * left[index]) + (right[index] * right[index]);
    }
  }

  return total_energy;
}

void TestOldestVoiceSteal()
{
  fantome::FantomeEngine engine;
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 64, 100));
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 67, 100));
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 71, 100));
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 74, 100));

  Expect(!HasActiveNote(engine, 60), "oldest voice should be stolen first");
  Expect(HasActiveNote(engine, 74), "new note should become active");
}

void TestProgramChangeLoadsPreset()
{
  fantome::FantomeEngine engine;
  engine.CurrentPatchMutable().master_volume = 0.42f;
  engine.SavePreset(2);
  engine.CurrentPatchMutable().master_volume = 0.91f;

  engine.HandleMidi(fantome::MidiMessage::ProgramChange(1, 2));

  Expect(std::fabs(engine.CurrentPatch().master_volume - 0.42f) < 0.0001f,
         "program change should reload the stored preset");
}

void TestCcMapping()
{
  fantome::FantomeEngine engine;
  engine.HandleMidi(fantome::MidiMessage::ControlChange(1, 74, 127));
  engine.HandleMidi(fantome::MidiMessage::ControlChange(1, 71, 64));
  engine.HandleMidi(fantome::MidiMessage::ControlChange(1, 13, 32));

  Expect(std::fabs(engine.CurrentPatch().filter.cutoff - 1.0f) < 0.0001f,
         "CC74 should map to filter cutoff");
  Expect(std::fabs(engine.CurrentPatch().filter.resonance - (64.0f / 127.0f)) < 0.0001f,
         "CC71 should map to filter resonance");
  Expect(std::fabs(engine.CurrentPatch().delay.mix - (32.0f / 127.0f)) < 0.0001f,
         "CC13 should map to delay mix");
}

void TestSustainBehavior()
{
  fantome::FantomeEngine engine;
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));
  engine.HandleMidi(fantome::MidiMessage::ControlChange(1, 64, 127));
  engine.HandleMidi(fantome::MidiMessage::NoteOff(1, 60));

  Expect(HasActiveNote(engine, 60), "voice should remain active while sustain is down");

  engine.HandleMidi(fantome::MidiMessage::ControlChange(1, 64, 0));
  Expect(!HasActiveNote(engine, 60), "voice should release when sustain goes up");
}

void TestMonoFallback()
{
  fantome::FantomeEngine engine;
  engine.CurrentPatchMutable().play_mode = fantome::PlayMode::Mono;
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 90));
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 67, 110));
  engine.HandleMidi(fantome::MidiMessage::NoteOff(1, 67));

  Expect(HasActiveNote(engine, 60), "mono mode should fall back to previous held note");
  Expect(!HasActiveNote(engine, 67), "released mono note should not stay active");
}

void TestRenderProducesAudio()
{
  fantome::FantomeEngine engine;
  engine.SetSampleRate(48000.0f);
  engine.CurrentPatchMutable().amp_env.attack_s = 0.001f;
  engine.CurrentPatchMutable().amp_env.decay_s = 0.05f;
  engine.CurrentPatchMutable().amp_env.sustain = 0.7f;
  engine.CurrentPatchMutable().chorus.mix = 0.0f;
  engine.CurrentPatchMutable().delay.mix = 0.0f;
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));

  std::vector<float> left(4096, 0.0f);
  std::vector<float> right(4096, 0.0f);
  engine.Render(left.data(), right.data(), left.size());

  Expect(BufferEnergy(left, right) > 0.0001f,
         "render should produce audible energy after note on");
}

void TestRenderReleaseFallsTowardSilence()
{
  fantome::FantomeEngine engine;
  engine.SetSampleRate(48000.0f);
  engine.CurrentPatchMutable().amp_env.attack_s = 0.001f;
  engine.CurrentPatchMutable().amp_env.decay_s = 0.01f;
  engine.CurrentPatchMutable().amp_env.sustain = 0.0f;
  engine.CurrentPatchMutable().amp_env.release_s = 0.01f;
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));

  std::vector<float> pre_release_left(1024, 0.0f);
  std::vector<float> pre_release_right(1024, 0.0f);
  engine.Render(
    pre_release_left.data(),
    pre_release_right.data(),
    pre_release_left.size());

  engine.HandleMidi(fantome::MidiMessage::NoteOff(1, 60));
  std::vector<float> post_release_left(4096, 0.0f);
  std::vector<float> post_release_right(4096, 0.0f);
  engine.Render(
    post_release_left.data(),
    post_release_right.data(),
    post_release_left.size());

  const auto tail_energy = BufferEnergy(post_release_left, post_release_right);
  Expect(tail_energy < 5.0f,
         "release tail should decay toward silence with a short release");
}

void TestLfoChangesRenderedSignal()
{
  fantome::FantomeEngine mod_engine;
  mod_engine.SetSampleRate(48000.0f);
  mod_engine.CurrentPatchMutable().noise_level = 0.0f;
  mod_engine.CurrentPatchMutable().osc_a.waveform = fantome::Waveform::Saw;
  mod_engine.CurrentPatchMutable().osc_b.level = 0.0f;
  mod_engine.CurrentPatchMutable().chorus.mix = 0.0f;
  mod_engine.CurrentPatchMutable().delay.mix = 0.0f;
  mod_engine.CurrentPatchMutable().reverb.mix = 0.0f;
  mod_engine.CurrentPatchMutable().amp_env.attack_s = 0.001f;
  mod_engine.CurrentPatchMutable().amp_env.decay_s = 0.05f;
  mod_engine.CurrentPatchMutable().amp_env.sustain = 0.8f;
  mod_engine.CurrentPatchMutable().osc_lfo.amount = 0.45f;
  mod_engine.CurrentPatchMutable().osc_lfo.rate_hz = 5.0f;
  mod_engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));

  std::vector<float> left(256, 0.0f);
  std::vector<float> right(256, 0.0f);
  float max_pitch_mod = 0.0f;

  for (int block = 0; block < 64; ++block) {
    mod_engine.Render(left.data(), right.data(), left.size());
    max_pitch_mod = std::max(
      max_pitch_mod,
      std::fabs(mod_engine.LastModulationFrame().osc_pitch_semitones));
  }

  Expect(max_pitch_mod > 0.05f, "pitch LFO should move the modulation frame over time");
}

void TestSampleAndHoldUpdatesModulationFrame()
{
  fantome::FantomeEngine engine;
  engine.SetSampleRate(48000.0f);
  engine.CurrentPatchMutable().filter_sample_hold.sync_mode = fantome::SyncMode::Free;
  engine.CurrentPatchMutable().filter_sample_hold.rate_hz = 12.0f;
  engine.CurrentPatchMutable().filter_sample_hold.amount = 0.25f;
  engine.CurrentPatchMutable().filter.sample_hold_amount = 0.25f;
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));

  std::vector<float> left(8192, 0.0f);
  std::vector<float> right(8192, 0.0f);
  engine.Render(left.data(), right.data(), left.size());

  Expect(std::fabs(engine.LastModulationFrame().filter_cutoff_delta) > 0.0001f,
         "sample and hold should eventually move the filter cutoff modulation");
}

void TestDelayExtendsTailEnergy()
{
  fantome::FantomeEngine dry_engine;
  dry_engine.SetSampleRate(48000.0f);
  dry_engine.CurrentPatchMutable().noise_level = 0.0f;
  dry_engine.CurrentPatchMutable().delay.mix = 0.0f;
  dry_engine.CurrentPatchMutable().chorus.mix = 0.0f;
  dry_engine.CurrentPatchMutable().amp_env.attack_s = 0.001f;
  dry_engine.CurrentPatchMutable().amp_env.decay_s = 0.02f;
  dry_engine.CurrentPatchMutable().amp_env.sustain = 0.0f;
  dry_engine.CurrentPatchMutable().amp_env.release_s = 0.01f;
  dry_engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));

  fantome::FantomeEngine wet_engine;
  wet_engine.SetSampleRate(48000.0f);
  wet_engine.CurrentPatchMutable().noise_level = 0.0f;
  wet_engine.CurrentPatchMutable().delay.sync_mode = fantome::SyncMode::Free;
  wet_engine.CurrentPatchMutable().delay.time_ms = 70.0f;
  wet_engine.CurrentPatchMutable().delay.feedback = 0.55f;
  wet_engine.CurrentPatchMutable().delay.mix = 0.38f;
  wet_engine.CurrentPatchMutable().chorus.mix = 0.0f;
  wet_engine.CurrentPatchMutable().amp_env.attack_s = 0.001f;
  wet_engine.CurrentPatchMutable().amp_env.decay_s = 0.02f;
  wet_engine.CurrentPatchMutable().amp_env.sustain = 0.0f;
  wet_engine.CurrentPatchMutable().amp_env.release_s = 0.01f;
  wet_engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));

  std::vector<float> lead_left(2048, 0.0f);
  std::vector<float> lead_right(2048, 0.0f);
  dry_engine.Render(lead_left.data(), lead_right.data(), lead_left.size());
  wet_engine.Render(lead_left.data(), lead_right.data(), lead_left.size());

  dry_engine.HandleMidi(fantome::MidiMessage::NoteOff(1, 60));
  wet_engine.HandleMidi(fantome::MidiMessage::NoteOff(1, 60));

  std::vector<float> dry_tail_left(8192, 0.0f);
  std::vector<float> dry_tail_right(8192, 0.0f);
  dry_engine.Render(dry_tail_left.data(), dry_tail_right.data(), dry_tail_left.size());

  std::vector<float> wet_tail_left(8192, 0.0f);
  std::vector<float> wet_tail_right(8192, 0.0f);
  wet_engine.Render(wet_tail_left.data(), wet_tail_right.data(), wet_tail_left.size());

  Expect(BufferEnergy(wet_tail_left, wet_tail_right) >
           (BufferEnergy(dry_tail_left, dry_tail_right) * 1.4f),
         "delay should keep more tail energy after note release");
}

void TestChorusChangesStereoSignal()
{
  fantome::FantomeEngine dry_engine;
  dry_engine.SetSampleRate(48000.0f);
  dry_engine.CurrentPatchMutable().noise_level = 0.0f;
  dry_engine.CurrentPatchMutable().chorus.mix = 0.0f;
  dry_engine.CurrentPatchMutable().delay.mix = 0.0f;
  dry_engine.CurrentPatchMutable().osc_b.level = 0.0f;
  dry_engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));

  fantome::FantomeEngine wet_engine;
  wet_engine.SetSampleRate(48000.0f);
  wet_engine.CurrentPatchMutable().noise_level = 0.0f;
  wet_engine.CurrentPatchMutable().chorus.depth = 0.75f;
  wet_engine.CurrentPatchMutable().chorus.rate_hz = 0.7f;
  wet_engine.CurrentPatchMutable().chorus.mix = 0.45f;
  wet_engine.CurrentPatchMutable().delay.mix = 0.0f;
  wet_engine.CurrentPatchMutable().osc_b.level = 0.0f;
  wet_engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));

  std::vector<float> dry_left(4096, 0.0f);
  std::vector<float> dry_right(4096, 0.0f);
  dry_engine.Render(dry_left.data(), dry_right.data(), dry_left.size());

  std::vector<float> wet_left(4096, 0.0f);
  std::vector<float> wet_right(4096, 0.0f);
  wet_engine.Render(wet_left.data(), wet_right.data(), wet_left.size());

  float diff = 0.0f;
  for (std::size_t index = 0; index < dry_left.size(); ++index) {
    diff += std::fabs(dry_left[index] - wet_left[index]);
    diff += std::fabs(dry_right[index] - wet_right[index]);
  }

  Expect(diff > 5.0f, "chorus should audibly modify the stereo signal");
}

void TestReverbAddsLateTail()
{
  fantome::FantomeEngine dry_engine;
  dry_engine.SetSampleRate(48000.0f);
  dry_engine.CurrentPatchMutable().noise_level = 0.0f;
  dry_engine.CurrentPatchMutable().chorus.mix = 0.0f;
  dry_engine.CurrentPatchMutable().delay.mix = 0.0f;
  dry_engine.CurrentPatchMutable().reverb.mix = 0.0f;
  dry_engine.CurrentPatchMutable().amp_env.attack_s = 0.001f;
  dry_engine.CurrentPatchMutable().amp_env.decay_s = 0.02f;
  dry_engine.CurrentPatchMutable().amp_env.sustain = 0.0f;
  dry_engine.CurrentPatchMutable().amp_env.release_s = 0.01f;
  dry_engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));

  fantome::FantomeEngine wet_engine;
  wet_engine.SetSampleRate(48000.0f);
  wet_engine.CurrentPatchMutable().noise_level = 0.0f;
  wet_engine.CurrentPatchMutable().chorus.mix = 0.0f;
  wet_engine.CurrentPatchMutable().delay.mix = 0.0f;
  wet_engine.CurrentPatchMutable().reverb.mix = 0.22f;
  wet_engine.CurrentPatchMutable().amp_env.attack_s = 0.001f;
  wet_engine.CurrentPatchMutable().amp_env.decay_s = 0.02f;
  wet_engine.CurrentPatchMutable().amp_env.sustain = 0.0f;
  wet_engine.CurrentPatchMutable().amp_env.release_s = 0.01f;
  wet_engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));

  std::vector<float> excite_left(4096, 0.0f);
  std::vector<float> excite_right(4096, 0.0f);
  dry_engine.Render(excite_left.data(), excite_right.data(), excite_left.size());
  wet_engine.Render(excite_left.data(), excite_right.data(), excite_left.size());

  dry_engine.HandleMidi(fantome::MidiMessage::NoteOff(1, 60));
  wet_engine.HandleMidi(fantome::MidiMessage::NoteOff(1, 60));

  std::vector<float> dry_tail_left(16384, 0.0f);
  std::vector<float> dry_tail_right(16384, 0.0f);
  dry_engine.Render(dry_tail_left.data(), dry_tail_right.data(), dry_tail_left.size());

  std::vector<float> wet_tail_left(16384, 0.0f);
  std::vector<float> wet_tail_right(16384, 0.0f);
  wet_engine.Render(wet_tail_left.data(), wet_tail_right.data(), wet_tail_left.size());

  Expect(BufferEnergy(wet_tail_left, wet_tail_right) >
           (BufferEnergy(dry_tail_left, dry_tail_right) * 1.2f),
         "reverb should add a late tail after the dry note has faded");
}

void TestOscillatorSyncChangesRenderedSignal()
{
  fantome::FantomeEngine free_engine;
  free_engine.SetSampleRate(48000.0f);
  free_engine.CurrentPatchMutable().noise_level = 0.0f;
  free_engine.CurrentPatchMutable().osc_a.waveform = fantome::Waveform::Saw;
  free_engine.CurrentPatchMutable().osc_b.waveform = fantome::Waveform::Saw;
  free_engine.CurrentPatchMutable().osc_b.octave = 1;
  free_engine.CurrentPatchMutable().chorus.mix = 0.0f;
  free_engine.CurrentPatchMutable().delay.mix = 0.0f;
  free_engine.CurrentPatchMutable().reverb.mix = 0.0f;
  free_engine.CurrentPatchMutable().amp_env.attack_s = 0.001f;
  free_engine.CurrentPatchMutable().amp_env.decay_s = 0.05f;
  free_engine.CurrentPatchMutable().amp_env.sustain = 0.9f;
  free_engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 48, 100));

  fantome::FantomeEngine sync_engine = free_engine;
  sync_engine.CurrentPatchMutable().osc_a.sync_enabled = true;
  sync_engine.CurrentPatchMutable().osc_b.sync_enabled = true;
  sync_engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 48, 100));

  std::vector<float> free_left(4096, 0.0f);
  std::vector<float> free_right(4096, 0.0f);
  free_engine.Render(free_left.data(), free_right.data(), free_left.size());

  std::vector<float> sync_left(4096, 0.0f);
  std::vector<float> sync_right(4096, 0.0f);
  sync_engine.Render(sync_left.data(), sync_right.data(), sync_left.size());

  float diff = 0.0f;
  for (std::size_t index = 0; index < free_left.size(); ++index) {
    diff += std::fabs(free_left[index] - sync_left[index]);
    diff += std::fabs(free_right[index] - sync_right[index]);
  }

  Expect(diff > 10.0f, "oscillator sync should audibly reshape the rendered signal");
}

void TestUnisonIsWiderThanMono()
{
  fantome::FantomeEngine mono_engine;
  mono_engine.SetSampleRate(48000.0f);
  mono_engine.CurrentPatchMutable().play_mode = fantome::PlayMode::Mono;
  mono_engine.CurrentPatchMutable().noise_level = 0.0f;
  mono_engine.CurrentPatchMutable().chorus.mix = 0.0f;
  mono_engine.CurrentPatchMutable().delay.mix = 0.0f;
  mono_engine.CurrentPatchMutable().reverb.mix = 0.0f;
  mono_engine.CurrentPatchMutable().amp_env.attack_s = 0.001f;
  mono_engine.CurrentPatchMutable().amp_env.decay_s = 0.02f;
  mono_engine.CurrentPatchMutable().amp_env.sustain = 0.9f;
  mono_engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));

  fantome::FantomeEngine unison_engine;
  unison_engine.SetSampleRate(48000.0f);
  unison_engine.CurrentPatchMutable().play_mode = fantome::PlayMode::Unison;
  unison_engine.CurrentPatchMutable().noise_level = 0.0f;
  unison_engine.CurrentPatchMutable().chorus.mix = 0.0f;
  unison_engine.CurrentPatchMutable().delay.mix = 0.0f;
  unison_engine.CurrentPatchMutable().reverb.mix = 0.0f;
  unison_engine.CurrentPatchMutable().amp_env.attack_s = 0.001f;
  unison_engine.CurrentPatchMutable().amp_env.decay_s = 0.02f;
  unison_engine.CurrentPatchMutable().amp_env.sustain = 0.9f;
  unison_engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));

  std::vector<float> mono_left(4096, 0.0f);
  std::vector<float> mono_right(4096, 0.0f);
  mono_engine.Render(mono_left.data(), mono_right.data(), mono_left.size());

  std::vector<float> unison_left(4096, 0.0f);
  std::vector<float> unison_right(4096, 0.0f);
  unison_engine.Render(unison_left.data(), unison_right.data(), unison_left.size());

  Expect(StereoDifference(unison_left, unison_right) >
           (StereoDifference(mono_left, mono_right) * 4.0f),
         "unison should create much more stereo width than mono mode");
}

void TestAggressiveEffectsStayBounded()
{
  fantome::FantomeEngine engine;
  engine.SetSampleRate(48000.0f);
  engine.CurrentPatchMutable().play_mode = fantome::PlayMode::Unison;
  engine.CurrentPatchMutable().noise_level = 0.08f;
  engine.CurrentPatchMutable().chorus.depth = 0.8f;
  engine.CurrentPatchMutable().chorus.mix = 0.55f;
  engine.CurrentPatchMutable().delay.sync_mode = fantome::SyncMode::Free;
  engine.CurrentPatchMutable().delay.time_ms = 240.0f;
  engine.CurrentPatchMutable().delay.feedback = 0.92f;
  engine.CurrentPatchMutable().delay.mix = 0.48f;
  engine.CurrentPatchMutable().reverb.mix = 0.32f;
  engine.CurrentPatchMutable().filter.resonance = 0.72f;
  engine.CurrentPatchMutable().amp_env.attack_s = 0.001f;
  engine.CurrentPatchMutable().amp_env.decay_s = 0.05f;
  engine.CurrentPatchMutable().amp_env.sustain = 0.75f;
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 120));

  std::vector<float> left(32768, 0.0f);
  std::vector<float> right(32768, 0.0f);
  engine.Render(left.data(), right.data(), left.size());

  Expect(std::isfinite(BufferEnergy(left, right)),
         "aggressive effect settings should still produce finite audio");
  Expect(MaxAbsSample(left, right) <= 1.0f,
         "aggressive effect settings should remain bounded by the output soft clip");
}

void TestRenderRemainsConsistentAcrossBlockSizes()
{
  fantome::FantomeEngine base_engine;
  base_engine.SetSampleRate(48000.0f);
  base_engine.CurrentPatchMutable() = fantome::MakeDefaultUserPresetBank()[0];
  base_engine.CurrentPatchMutable().amp_env.attack_s = 0.001f;
  base_engine.CurrentPatchMutable().delay.mix = 0.18f;
  base_engine.CurrentPatchMutable().reverb.mix = 0.16f;
  base_engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 104));

  const auto uniform_energy = RenderPlanEnergy(
    base_engine,
    std::vector<std::size_t>(64, 64));
  const auto tiny_blocks_energy = RenderPlanEnergy(
    base_engine,
    std::vector<std::size_t>(256, 16));
  const auto mixed_blocks_energy = RenderPlanEnergy(
    base_engine,
    std::vector<std::size_t> {
      23, 71, 19, 128, 64, 7, 257, 33, 41, 51, 89, 241, 73, 11, 71, 286,
      17, 95, 63, 127, 255, 31, 49, 113, 197, 83, 29, 401, 257, 311, 263, 340});

  Expect(uniform_energy > 0.001f,
         "reference block plan should produce audible energy");
  Expect(std::fabs(tiny_blocks_energy - uniform_energy) / uniform_energy < 0.12f,
         "render energy should stay reasonably stable across very small blocks");
  Expect(std::fabs(mixed_blocks_energy - uniform_energy) / uniform_energy < 0.20f,
         "render energy should stay reasonably stable across irregular block plans");
}

void TestRealisticMidiScenarioStaysFinite()
{
  fantome::FantomeEngine engine;
  engine.SetSampleRate(48000.0f);
  engine.CurrentPatchMutable() = fantome::MakeDefaultUserPresetBank()[3];
  engine.HandleMidi(fantome::MidiMessage::Start());
  for (int index = 0; index < 48; ++index) {
    engine.HandleMidi(fantome::MidiMessage::Clock());
  }

  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 112));
  engine.HandleMidi(fantome::MidiMessage::ControlChange(1, 1, 72));
  engine.HandleMidi(fantome::MidiMessage::PitchBend(1, 4096));
  engine.HandleMidi(fantome::MidiMessage::ControlChange(1, 64, 127));
  engine.HandleMidi(fantome::MidiMessage::NoteOff(1, 60));
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 67, 104));
  engine.HandleMidi(fantome::MidiMessage::ControlChange(1, 74, 92));
  engine.HandleMidi(fantome::MidiMessage::ControlChange(1, 64, 0));

  const auto energy = RenderPlanEnergy(
    engine,
    std::vector<std::size_t> {64, 64, 32, 96, 48, 48, 128, 64, 32, 16, 16, 128});

  Expect(std::isfinite(energy) && energy > 0.0001f,
         "a realistic mixed MIDI performance scenario should keep the engine finite and audible");
  Expect(HasActiveNote(engine, 67),
         "the second played note should remain active after the scenario");
}

void TestUiEncoderEditsFilterCutoff()
{
  fantome::FantomeEngine engine;
  fantome::UiState ui;
  ui.Reset(engine);
  ui.NextPage(engine);

  const auto before = engine.CurrentPatch().filter.cutoff;
  ui.PressEncoder(engine);
  ui.RotateEncoder(3, engine);
  ui.PressEncoder(engine);

  Expect(engine.CurrentPatch().filter.cutoff > before,
         "encoder editing should change the selected parameter");
}

void TestSoftTakeoverBlocksUntilPickup()
{
  fantome::FantomeEngine engine;
  fantome::UiState ui;
  ui.Reset(engine);

  ui.MovePot(1, 0.20f, engine);
  engine.CurrentPatchMutable().filter.cutoff = 0.85f;
  ui.NotifyPresetLoaded(engine);

  ui.MovePot(1, 0.25f, engine);
  Expect(std::fabs(engine.CurrentPatch().filter.cutoff - 0.85f) < 0.0001f,
         "soft takeover should block abrupt jumps after preset load");

  ui.MovePot(1, 0.84f, engine);
  Expect(std::fabs(engine.CurrentPatch().filter.cutoff - 0.84f) < 0.02f,
         "soft takeover should engage once the physical pot reaches the logical value");
}

void TestPresetPersistenceRoundTrip()
{
  const auto preset_path = std::filesystem::temp_directory_path() / "fantome_iv_presets_test.txt";
  std::filesystem::remove(preset_path);

  fantome::FantomeEngine source_engine;
  source_engine.CurrentPatchMutable().filter.cutoff = 0.41f;
  source_engine.CurrentPatchMutable().delay.feedback = 0.57f;
  source_engine.CurrentPatchMutable().reverb.mix = 0.19f;
  source_engine.CurrentPatchMutable().name = "Roundtrip";
  source_engine.SavePreset(2);

  Expect(source_engine.SavePresetBankToFile(preset_path.string()),
         "preset bank should save to disk");

  fantome::FantomeEngine loaded_engine;
  Expect(loaded_engine.LoadPresetBankFromFile(preset_path.string()),
         "preset bank should load from disk");

  Expect(loaded_engine.CurrentPresetSlot() == 2,
         "active preset slot should survive persistence");
  Expect(loaded_engine.CurrentPatch().name == "Roundtrip",
         "preset name should survive persistence");
  Expect(std::fabs(loaded_engine.CurrentPatch().filter.cutoff - 0.41f) < 0.0001f,
         "filter cutoff should survive persistence");
  Expect(std::fabs(loaded_engine.CurrentPatch().delay.feedback - 0.57f) < 0.0001f,
         "delay feedback should survive persistence");
  Expect(std::fabs(loaded_engine.CurrentPatch().reverb.mix - 0.19f) < 0.0001f,
         "reverb mix should survive persistence");

  std::filesystem::remove(preset_path);
}

void TestCurrentPresetDirtyStateTracksEdits()
{
  fantome::FantomeEngine engine;

  Expect(!engine.IsCurrentPresetDirty(),
         "freshly loaded user slot should start in a saved state");

  engine.CurrentPatchMutable().filter.cutoff = 0.41f;
  Expect(engine.IsCurrentPresetDirty(),
         "editing the current patch should mark the active user slot as dirty");

  engine.SavePreset(engine.CurrentPresetSlot());
  Expect(!engine.IsCurrentPresetDirty(),
         "writing the patch back into the active user slot should clear the dirty state");
}

void TestOledRendererShowsPresetAndSelection()
{
  fantome::FantomeEngine engine;
  fantome::UiState ui;
  fantome::OledTextRenderer oled;
  ui.Reset(engine);

  const auto frame = oled.Render(ui, engine);
  const auto debug = frame.ToDebugString();

  Expect(debug.find("Ghost Pad") != std::string::npos,
         "oled view should show the active preset name");
  Expect(debug.find("Oscillators") != std::string::npos,
         "oled view should show the current page label");
  Expect(debug.find(">Osc A Wave") != std::string::npos,
         "oled view should highlight the selected parameter");
  Expect(debug.find("Saw") != std::string::npos,
         "oled view should show the formatted parameter value");
}

void TestOledRendererShowsStartupSplash()
{
  fantome::OledTextRenderer oled;
  const auto frame = oled.RenderStartupSplash();
  const auto debug = frame.ToDebugString();

  Expect(debug.find("FANTOME IV") != std::string::npos,
         "startup splash should show the synth name");
  Expect(debug.find("by Dr. John.") != std::string::npos,
         "startup splash should show the author line");
}

void TestOledRendererShowsPickupState()
{
  fantome::FantomeEngine engine;
  fantome::UiState ui;
  fantome::OledTextRenderer oled;
  ui.Reset(engine);

  ui.MovePot(1, 0.20f, engine);
  engine.CurrentPatchMutable().filter.cutoff = 0.85f;
  ui.NotifyPresetLoaded(engine);

  const auto frame = oled.Render(ui, engine);
  const auto debug = frame.ToDebugString();

  Expect(debug.find("Pickup") != std::string::npos,
         "oled view should expose soft takeover pickup state");
}

void TestOledRendererShowsConfirmationState()
{
  fantome::FantomeEngine engine;
  fantome::UiState ui;
  fantome::OledTextRenderer oled;
  ui.Reset(engine);

  for (int index = 0; index < 6; ++index) {
    ui.NextPage(engine);
  }
  ui.RotateEncoder(5, engine);
  ui.PressEncoder(engine);

  const auto frame = oled.Render(ui, engine);
  const auto debug = frame.ToDebugString();

  Expect(debug.find("CONF") != std::string::npos,
         "oled view should show confirmation mode for sensitive actions");
  Expect(debug.find("Write Slot") != std::string::npos,
         "oled view should show the selected action label");
  Expect(debug.find("Enc=ok") != std::string::npos,
         "oled view should show confirmation guidance");
}

void TestOledRendererShowsPresetSessionBrowserOnSystemPage()
{
  const auto session_path =
    std::filesystem::temp_directory_path() / "fantome_iv_oled_browser_session.txt";
  std::filesystem::remove(session_path);

  fantome::FantomeEngine engine;
  fantome::UiState ui;
  fantome::OledTextRenderer oled;
  fantome::SessionManager manager;
  manager.Boot(session_path.string(), engine, ui);

  for (int index = 0; index < 6; ++index) {
    ui.NextPage(engine);
  }
  ui.RotateEncoder(3, engine);
  ui.PressEncoder(engine);
  ui.RotateEncoder(2, engine);
  ui.PressEncoder(engine);

  const auto debug = oled.Render(ui, engine, &manager.State()).ToDebugString();

  Expect(debug.find("User Slots") != std::string::npos,
         "system page should switch to the user-slot browser header");
  Expect(debug.find("*P1 Ghost Pad") != std::string::npos,
         "system browser should show the active preset");
  Expect(debug.find(">P3 Phantom Brass") != std::string::npos,
         "system browser should highlight the target preset slot");
  Expect(debug.find("Sess:fresh on") != std::string::npos,
         "system browser should show current session status");

  std::filesystem::remove(session_path);
}

void TestOledRendererShowsDirtyUserSlotState()
{
  fantome::FantomeEngine engine;
  fantome::UiState ui;
  fantome::OledTextRenderer oled;
  ui.Reset(engine);

  engine.CurrentPatchMutable().filter.cutoff = 0.42f;

  const auto debug = oled.Render(ui, engine).ToDebugString();
  Expect(debug.find("P1* Ghost Pad") != std::string::npos,
         "oled header should show when the current user slot has unsaved changes");
}

void TestPortableSessionPersistenceRoundTrip()
{
  const auto session_path = std::filesystem::temp_directory_path() / "fantome_iv_session_test.txt";
  std::filesystem::remove(session_path);

  fantome::FantomeEngine source_engine;
  fantome::UiState source_ui;
  fantome::PortableInputSurface input;
  source_ui.Reset(source_engine);

  source_engine.CurrentPatchMutable().name = "Session Drift";
  source_engine.CurrentPatchMutable().filter.cutoff = 0.61f;
  source_engine.SavePreset(2);
  source_engine.CurrentPatchMutable().filter.cutoff = 0.29f;
  source_engine.CurrentPatchMutable().delay.feedback = 0.82f;
  source_engine.SetClockTempoBpm(98.0f);

  for (int index = 0; index < 5; ++index) {
    input.PressPageNext(source_engine, source_ui);
  }
  input.TurnEncoder(5, source_engine, source_ui);
  input.MovePot(7, 0.12f, source_engine, source_ui);
  source_engine.CurrentPatchMutable().delay.feedback = 0.82f;
  source_ui.NotifyPresetLoaded(source_engine);

  const fantome::PortableSessionState state {
    source_engine.ExportSessionState(),
    source_ui.ExportSessionState(),
  };

  Expect(fantome::SessionPersistence::Save(session_path.string(), state),
         "portable session should save to disk");

  fantome::PortableSessionState loaded_state;
  Expect(fantome::SessionPersistence::Load(session_path.string(), loaded_state),
         "portable session should load from disk");

  fantome::FantomeEngine restored_engine;
  fantome::UiState restored_ui;
  fantome::OledTextRenderer oled;
  Expect(restored_engine.RestoreSessionState(loaded_state.engine),
         "engine session state should restore");
  restored_ui.RestoreSessionState(loaded_state.ui, restored_engine);

  Expect(restored_engine.CurrentPresetSlot() == 2,
         "session should restore the active preset slot");
  Expect(std::fabs(restored_engine.CurrentPatch().filter.cutoff - 0.29f) < 0.0001f,
         "session should restore the current unsaved patch state");
  Expect(std::fabs(restored_engine.PresetBank()[2].filter.cutoff - 0.61f) < 0.0001f,
         "session should preserve the stored preset bank independently");
  Expect(std::fabs(restored_engine.Transport().tempo_bpm - 98.0f) < 0.0001f,
         "session should restore the working clock tempo");
  Expect(restored_ui.CurrentPage() == fantome::UiPage::Effects,
         "session should restore the current UI page");
  Expect(restored_ui.SelectedItemIndex() == 5,
         "session should restore the selected UI item");
  Expect(!restored_ui.Pots()[7].captured,
         "session should restore soft takeover state for the context pot");

  const auto debug = oled.Render(restored_ui, restored_engine).ToDebugString();
  Expect(debug.find("Pickup") != std::string::npos,
         "restored session should expose pickup state on the OLED");

  std::filesystem::remove(session_path);
}

void TestPortableInputSurfaceCanNavigateAndEdit()
{
  fantome::FantomeEngine engine;
  fantome::UiState ui;
  fantome::PortableInputSurface input;
  ui.Reset(engine);

  Expect(input.Apply(fantome::ControlEvent::PageNext(), engine, ui),
         "page-next event should be accepted");
  Expect(ui.CurrentPage() == fantome::UiPage::Filter,
         "portable input should navigate to the next page");

  const auto before = engine.CurrentPatch().filter.cutoff;
  input.Apply(fantome::ControlEvent::EncoderPress(), engine, ui);
  input.Apply(fantome::ControlEvent::EncoderTurn(4), engine, ui);
  input.Apply(fantome::ControlEvent::EncoderPress(), engine, ui);

  Expect(engine.CurrentPatch().filter.cutoff > before,
         "portable input should edit the selected parameter through the encoder");

  Expect(input.Apply(fantome::ControlEvent::PotMove(1, 0.18f), engine, ui),
         "pot move event should be accepted");
  Expect(std::fabs(engine.CurrentPatch().filter.cutoff - 0.18f) < 0.02f,
         "portable input should route pot movement to the mapped parameter");
  Expect(std::fabs(input.PotPositions()[1] - 0.18f) < 0.0001f,
         "portable input should retain the last simulated pot position");
  Expect(input.EncoderPosition() == 4,
         "portable input should track cumulative encoder motion");
}

void TestHardwareControlRouterBridgesRawControls()
{
  fantome::FantomeEngine engine;
  fantome::UiState ui;
  fantome::HardwareControlRouter router;
  ui.Reset(engine);

  fantome::HardwareInputFrame frame;
  frame.buttons[static_cast<std::size_t>(fantome::HardwareButtonId::PageNext)].just_pressed = true;
  Expect(router.Apply(frame, engine, ui),
         "hardware router should accept raw page-next button presses");
  Expect(ui.CurrentPage() == fantome::UiPage::Filter,
         "hardware router should move the UI to the next page");

  frame.ClearTransient();
  frame.buttons[static_cast<std::size_t>(fantome::HardwareButtonId::Encoder)].just_pressed = true;
  Expect(router.Apply(frame, engine, ui),
         "hardware router should translate encoder clicks");

  frame.ClearTransient();
  frame.encoder_delta = 4;
  Expect(router.Apply(frame, engine, ui),
         "hardware router should translate encoder turns");

  frame.ClearTransient();
  frame.buttons[static_cast<std::size_t>(fantome::HardwareButtonId::Encoder)].just_pressed = true;
  router.Apply(frame, engine, ui);

  frame.ClearTransient();
  frame.pots[1] = {true, 0.22f};
  Expect(router.Apply(frame, engine, ui),
         "hardware router should forward available pots to the UI mapping");
  Expect(std::fabs(engine.CurrentPatch().filter.cutoff - 0.22f) < 0.02f,
         "hardware router should preserve the fixed cutoff mapping on pot 2");
}

void TestHardwareControlScannerFiltersRawInput()
{
  fantome::HardwareControlScanner scanner;
  fantome::RawHardwareInputFrame raw;
  fantome::HardwareInputFrame cooked;

  raw.pot_available[1] = true;
  raw.pots[1] = 0.0f;
  Expect(scanner.Scan(raw, cooked),
         "scanner should report a first available potentiometer sample");
  Expect(cooked.pots[1].available,
         "scanner should expose available pots");

  raw.ClearTransient();
  raw.pot_available[1] = true;
  raw.pots[1] = 1.0f;
  raw.buttons[static_cast<std::size_t>(fantome::HardwareButtonId::PageNext)] = true;
  raw.PushMidi(fantome::MidiMessage::NoteOn(1, 60, 100));
  Expect(scanner.Scan(raw, cooked),
         "scanner should surface button edges, MIDI and filtered pot motion");
  Expect(cooked.buttons[static_cast<std::size_t>(fantome::HardwareButtonId::PageNext)].just_pressed,
         "scanner should detect just-pressed button edges");
  Expect(cooked.midi_message_count == 1,
         "scanner should forward queued MIDI messages");
  Expect(cooked.pots[1].normalized > 0.2f && cooked.pots[1].normalized < 1.0f,
         "scanner should smooth large pot jumps instead of teleporting");
}

void TestFirmwareRuntimeTracksDirtyPresetAndSession()
{
  const auto session_path =
    std::filesystem::temp_directory_path() / "fantome_iv_runtime_session_test.txt";
  std::filesystem::remove(session_path);

  fantome::FirmwareRuntime runtime;
  const auto boot = runtime.BootWithSession(session_path.string(), 48000.0f);

  Expect(!boot.standalone, "runtime should boot in session-backed mode");
  auto output = runtime.BuildHardwareOutputFrame();
  Expect(!output.preset_dirty,
         "freshly booted runtime should begin with a clean active user slot");
  Expect(!output.session_dirty,
         "freshly booted runtime should begin with a clean session checkpoint state");

  fantome::HardwareInputFrame frame;
  frame.pots[1] = {true, 0.31f};
  Expect(runtime.ApplyHardwareFrame(frame),
         "runtime should accept raw hardware frames");

  output = runtime.BuildHardwareOutputFrame();
  Expect(output.preset_dirty,
         "runtime should surface unsaved preset edits after a pot move");
  Expect(output.session_dirty,
         "runtime should mark the session dirty after an edit");

  Expect(runtime.SaveSessionCheckpoint(),
         "runtime should expose explicit checkpoint saves");
  output = runtime.BuildHardwareOutputFrame();
  Expect(!output.session_dirty,
         "saving a checkpoint should clear the session dirty flag");

  Expect(runtime.Shutdown(),
         "runtime should save successfully on shutdown");
  std::filesystem::remove(session_path);
}

void TestFirmwareRuntimeRenderAudioBlockTracksMeter()
{
  fantome::FirmwareRuntime runtime;
  runtime.BootStandalone(48000.0f);
  runtime.Engine().CurrentPatchMutable().amp_env.attack_s = 0.001f;
  runtime.Engine().CurrentPatchMutable().amp_env.decay_s = 0.02f;
  runtime.Engine().CurrentPatchMutable().amp_env.sustain = 0.7f;
  runtime.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 110));

  std::vector<float> left(2048, 0.0f);
  std::vector<float> right(2048, 0.0f);
  fantome::HardwareAudioBuffer buffer {left.data(), right.data(), left.size()};
  runtime.Render(buffer);

  const auto output = runtime.BuildHardwareOutputFrame();
  Expect(output.audio_block_count == 1,
         "runtime should count rendered audio blocks");
  Expect(output.output_peak > 0.001f,
         "runtime should expose a non-zero output peak after rendering audio");
}

void TestFirmwareRuntimeUiSaveAndReloadSessionActions()
{
  const auto session_path =
    std::filesystem::temp_directory_path() / "fantome_iv_runtime_ui_session_test.txt";
  std::filesystem::remove(session_path);

  fantome::FirmwareRuntime runtime;
  runtime.BootWithSession(session_path.string(), 48000.0f);

  fantome::HardwareInputFrame frame;
  frame.pots[1] = {true, 0.36f};
  runtime.ApplyHardwareFrame(frame);

  for (int index = 0; index < 6; ++index) {
    frame.ClearTransient();
    frame.buttons[static_cast<std::size_t>(fantome::HardwareButtonId::PageNext)].just_pressed = true;
    runtime.ApplyHardwareFrame(frame);
  }

  frame.ClearTransient();
  frame.encoder_delta = 6;
  runtime.ApplyHardwareFrame(frame);
  frame.ClearTransient();
  frame.buttons[static_cast<std::size_t>(fantome::HardwareButtonId::Encoder)].just_pressed = true;
  runtime.ApplyHardwareFrame(frame);
  frame.ClearTransient();
  frame.buttons[static_cast<std::size_t>(fantome::HardwareButtonId::Encoder)].just_pressed = true;
  runtime.ApplyHardwareFrame(frame);

  runtime.AdvanceDisplay(1.6f);
  auto output = runtime.BuildHardwareOutputFrame();
  Expect(!output.session_dirty,
         "save-session action from the system page should clear the session dirty flag");
  Expect(output.oled.ToDebugString().find("Session saved") != std::string::npos,
         "save-session action should surface a transient confirmation message");

  runtime.Engine().CurrentPatchMutable().filter.cutoff = 0.82f;
  runtime.Engine().CurrentPatchMutable().name = "Unsaved";
  frame.ClearTransient();
  frame.encoder_delta = 1;
  runtime.ApplyHardwareFrame(frame);
  frame.ClearTransient();
  frame.buttons[static_cast<std::size_t>(fantome::HardwareButtonId::Encoder)].just_pressed = true;
  runtime.ApplyHardwareFrame(frame);
  frame.ClearTransient();
  frame.buttons[static_cast<std::size_t>(fantome::HardwareButtonId::Encoder)].just_pressed = true;
  runtime.ApplyHardwareFrame(frame);

  Expect(std::fabs(runtime.Engine().CurrentPatch().filter.cutoff - 0.36f) < 0.02f,
         "reload-session action should restore the last saved session patch state");
  Expect(runtime.Engine().CurrentPatch().name == "Ghost Pad",
         "reload-session action should restore the last saved session patch name");
  runtime.AdvanceDisplay(0.01f);
  output = runtime.BuildHardwareOutputFrame();
  Expect(output.oled.ToDebugString().find("Session restored") != std::string::npos,
         "reload-session action should surface a transient restore message");

  runtime.AdvanceDisplay(2.0f);
  output = runtime.BuildHardwareOutputFrame();
  Expect(output.oled.ToDebugString().find("Session restored") == std::string::npos,
         "transient session message should expire after a short hold time");

  runtime.Shutdown();
  std::filesystem::remove(session_path);
}

void TestDaisyStubAppBridgesQueuedInputAndAudio()
{
  fantome::DaisyPlatformStub platform;
  fantome::DaisyApp app(platform);
  app.BootStandalone();

  fantome::RawHardwareInputFrame raw;
  raw.pot_available[1] = true;
  raw.pots[1] = 0.42f;
  platform.AdcStub().QueueFrame(raw);
  platform.MidiStub().QueueMessage(fantome::MidiMessage::NoteOn(1, 60, 110));
  platform.MidiStub().QueueMessage(fantome::MidiMessage::ControlChange(1, 74, 80));

  Expect(app.TickControlFrame(0.01f),
         "daisy app should consume queued stub input");
  Expect(platform.LastOutput().midi_activity,
         "queued MIDI should light the runtime MIDI activity state");
  Expect(platform.OledStub().PresentCount() >= 1,
         "daisy app should present OLED frames through the dedicated OLED stub");

  std::vector<float> left(1024, 0.0f);
  std::vector<float> right(1024, 0.0f);
  fantome::HardwareAudioBuffer buffer {left.data(), right.data(), left.size()};
  app.RenderAudioBlock(buffer);

  Expect(BufferEnergy(left, right) > 0.0001f,
         "daisy stub app should render audible audio through the runtime");
  Expect(platform.LastOutput().audio_block_count == 1,
         "daisy stub app should surface rendered audio block metrics");
  Expect(platform.OledStub().LastFrame().ToDebugString().find("FANTOME IV") != std::string::npos ||
           platform.OledStub().LastFrame().ToDebugString().find("Ghost Pad") != std::string::npos,
         "daisy app should route OLED content through the dedicated display stub");
}

void TestDaisyMidiUartStubParsesByteStream()
{
  fantome::DaisyMidiUartStub midi_stub;
  midi_stub.Init();

  midi_stub.QueueBytes({
    0x90, 0x3C, 0x64,
    0xB0, 0x4A, 0x50,
    0xE0, 0x00, 0x60,
    0xF8,
    0x80, 0x3C, 0x40,
  });

  fantome::RawHardwareInputFrame raw;
  Expect(midi_stub.Drain(raw),
         "MIDI UART stub should parse queued bytes into MIDI messages");
  Expect(raw.midi_message_count == 5,
         "byte-stream parser should emit channel voice and realtime MIDI messages");
  Expect(raw.midi_messages[0].type == fantome::MidiMessageType::NoteOn,
         "parsed byte stream should begin with note on");
  Expect(raw.midi_messages[1].type == fantome::MidiMessageType::ControlChange,
         "parsed byte stream should include control change");
  Expect(raw.midi_messages[2].type == fantome::MidiMessageType::PitchBend,
         "parsed byte stream should include pitch bend");
  Expect(raw.midi_messages[3].type == fantome::MidiMessageType::Clock,
         "parsed byte stream should include realtime clock");
  Expect(raw.midi_messages[4].type == fantome::MidiMessageType::NoteOff,
         "parsed byte stream should end with note off");
}

void TestDaisyAdcStubAcceptsRawPotSamples()
{
  fantome::DaisyPlatformStub platform;
  fantome::DaisyApp app(platform);
  app.BootStandalone();

  platform.AdcStub().QueuePotRawSample(1, 3072);
  Expect(app.TickControlFrame(0.01f),
         "ADC stub should convert queued raw pot samples into a control frame");
  Expect(app.Runtime().Engine().CurrentPatch().filter.cutoff > 0.6f,
         "raw ADC pot sample should reach the mapped cutoff parameter");
}

void TestDaisyStubSupportsCustomAudioConfigAndSeparatePeripherals()
{
  fantome::DaisyPlatformStub platform;
  platform.SetAudioConfig(fantome::DaisyAudioConfig {44100.0f, 96});
  fantome::DaisyApp app(platform);
  const auto boot = app.BootStandalone();

  Expect(boot.standalone,
         "daisy stub should still boot standalone with a custom audio config");
  Expect(std::fabs(app.Runtime().Engine().SampleRate() - 44100.0f) < 0.001f,
         "custom stub audio config should feed the runtime sample rate");

  fantome::RawHardwareInputFrame control_frame;
  control_frame.buttons[static_cast<std::size_t>(fantome::HardwareButtonId::PageNext)] = true;
  platform.AdcStub().QueueFrame(control_frame);
  platform.MidiStub().QueueMessage(fantome::MidiMessage::NoteOn(1, 64, 100));

  Expect(app.TickControlFrame(0.01f),
         "separate ADC and MIDI stubs should both contribute to a control tick");
  Expect(platform.OledStub().PresentCount() >= 1,
         "OLED stub should receive presented frames when the app ticks");
}

void TestSessionManagerStartsFreshWhenNoSessionFile()
{
  const auto session_path =
    std::filesystem::temp_directory_path() / "fantome_iv_session_manager_missing.txt";
  std::filesystem::remove(session_path);

  fantome::FantomeEngine engine;
  fantome::UiState ui;
  fantome::SessionManager manager;

  const auto result = manager.Boot(session_path.string(), engine, ui);

  Expect(result.mode == fantome::SessionBootMode::FreshStart,
         "session manager should start fresh when no session file exists");
  Expect(!result.session_file_present,
         "missing session file should be reported as absent");
  Expect(engine.CurrentPatch().name == "Ghost Pad",
         "fresh boot should start from the default first user slot");
}

void TestSessionManagerShutdownAndRestoreRoundTrip()
{
  const auto session_path =
    std::filesystem::temp_directory_path() / "fantome_iv_session_manager_roundtrip.txt";
  std::filesystem::remove(session_path);

  {
    fantome::FantomeEngine engine;
    fantome::UiState ui;
    fantome::PortableInputSurface input;
    fantome::SessionManager manager;

    const auto boot = manager.Boot(session_path.string(), engine, ui);
    Expect(boot.mode == fantome::SessionBootMode::FreshStart,
           "first boot should start a fresh session");

    engine.CurrentPatchMutable().name = "Manager Session";
    engine.CurrentPatchMutable().filter.cutoff = 0.57f;
    engine.CurrentPatchMutable().delay.feedback = 0.63f;
    engine.SetClockTempoBpm(104.0f);
    input.PressPageNext(engine, ui);
    input.PressPageNext(engine, ui);
    input.TurnEncoder(2, engine, ui);
    Expect(manager.SaveCheckpoint(engine, ui),
           "session manager should support explicit checkpoint saves");
    Expect(manager.Shutdown(engine, ui),
           "session manager should save the session on shutdown");
  }

  {
    fantome::FantomeEngine restored_engine;
    fantome::UiState restored_ui;
    fantome::SessionManager manager;

    const auto boot = manager.Boot(session_path.string(), restored_engine, restored_ui);
    Expect(boot.mode == fantome::SessionBootMode::RestoredFromDisk,
           "session manager should restore a saved session on boot");
    Expect(boot.restored,
           "restored boot should report a restored session");
    Expect(std::fabs(restored_engine.CurrentPatch().filter.cutoff - 0.57f) < 0.0001f,
           "restored session should recover the patch state");
    Expect(std::fabs(restored_engine.CurrentPatch().delay.feedback - 0.63f) < 0.0001f,
           "restored session should recover effect settings");
    Expect(std::fabs(restored_engine.Transport().tempo_bpm - 104.0f) < 0.0001f,
           "restored session should recover the working tempo");
    Expect(restored_ui.CurrentPage() == fantome::UiPage::AmpEnvelope,
           "restored session should recover the current UI page");
    Expect(restored_ui.SelectedItemIndex() == 2,
           "restored session should recover encoder selection");
  }

  std::filesystem::remove(session_path);
}

void TestSessionManagerFallsBackWhenSessionIsCorrupt()
{
  const auto session_path =
    std::filesystem::temp_directory_path() / "fantome_iv_session_manager_corrupt.txt";
  {
    std::ofstream output(session_path);
    output << "NOT_A_VALID_SESSION\n";
  }

  fantome::FantomeEngine engine;
  fantome::UiState ui;
  fantome::SessionManager manager;

  const auto result = manager.Boot(session_path.string(), engine, ui);

  Expect(result.mode == fantome::SessionBootMode::FallbackFreshStart,
         "corrupt session files should fall back to a fresh session");
  Expect(result.session_file_present,
         "corrupt boot should still report that a session file existed");
  Expect(manager.State().last_error == "session restore failed",
         "session manager should retain the restore failure reason");

  std::filesystem::remove(session_path);
}

void TestStartupDisplayShowsSplashThenMainScreen()
{
  fantome::FantomeEngine engine;
  fantome::UiState ui;
  fantome::OledTextRenderer oled;
  fantome::StartupDisplayController startup(1.5f);
  ui.Reset(engine);

  const auto splash_debug = startup.Render(oled, ui, engine).ToDebugString();
  Expect(startup.ShowingSplash(),
         "startup display should begin on the splash screen");
  Expect(splash_debug.find("FANTOME IV") != std::string::npos,
         "startup display should render the splash text while active");

  startup.Advance(1.6f);

  const auto main_debug = startup.Render(oled, ui, engine).ToDebugString();
  Expect(!startup.ShowingSplash(),
         "startup display should leave the splash after the configured duration");
  Expect(main_debug.find("Ghost Pad") != std::string::npos,
         "startup display should fall through to the main OLED screen afterward");
}

void TestStartupDisplayResetRearmsSplash()
{
  fantome::FantomeEngine engine;
  fantome::UiState ui;
  fantome::OledTextRenderer oled;
  fantome::StartupDisplayController startup(1.5f);
  ui.Reset(engine);

  startup.Advance(2.0f);
  Expect(!startup.ShowingSplash(),
         "startup display should be past the splash before reset");

  startup.Reset();
  const auto debug = startup.Render(oled, ui, engine).ToDebugString();
  Expect(startup.ShowingSplash(),
         "startup reset should rearm the splash screen");
  Expect(debug.find("by Dr. John.") != std::string::npos,
         "startup reset should make the splash visible again");
}

}  // namespace

int main()
{
  try {
    TestOldestVoiceSteal();
    TestProgramChangeLoadsPreset();
    TestCcMapping();
    TestSustainBehavior();
    TestMonoFallback();
    TestRenderProducesAudio();
    TestRenderReleaseFallsTowardSilence();
    TestLfoChangesRenderedSignal();
    TestSampleAndHoldUpdatesModulationFrame();
    TestDelayExtendsTailEnergy();
    TestChorusChangesStereoSignal();
    TestReverbAddsLateTail();
    TestOscillatorSyncChangesRenderedSignal();
    TestUnisonIsWiderThanMono();
    TestAggressiveEffectsStayBounded();
    TestRenderRemainsConsistentAcrossBlockSizes();
    TestRealisticMidiScenarioStaysFinite();
    TestUiEncoderEditsFilterCutoff();
    TestSoftTakeoverBlocksUntilPickup();
    TestPresetPersistenceRoundTrip();
    TestCurrentPresetDirtyStateTracksEdits();
    TestOledRendererShowsStartupSplash();
    TestOledRendererShowsPresetAndSelection();
    TestOledRendererShowsPickupState();
    TestOledRendererShowsConfirmationState();
    TestOledRendererShowsPresetSessionBrowserOnSystemPage();
    TestOledRendererShowsDirtyUserSlotState();
    TestPortableSessionPersistenceRoundTrip();
    TestPortableInputSurfaceCanNavigateAndEdit();
    TestHardwareControlRouterBridgesRawControls();
    TestHardwareControlScannerFiltersRawInput();
    TestFirmwareRuntimeTracksDirtyPresetAndSession();
    TestFirmwareRuntimeRenderAudioBlockTracksMeter();
    TestFirmwareRuntimeUiSaveAndReloadSessionActions();
    TestDaisyStubAppBridgesQueuedInputAndAudio();
    TestDaisyMidiUartStubParsesByteStream();
    TestDaisyAdcStubAcceptsRawPotSamples();
    TestDaisyStubSupportsCustomAudioConfigAndSeparatePeripherals();
    TestSessionManagerStartsFreshWhenNoSessionFile();
    TestSessionManagerShutdownAndRestoreRoundTrip();
    TestSessionManagerFallsBackWhenSessionIsCorrupt();
    TestStartupDisplayShowsSplashThenMainScreen();
    TestStartupDisplayResetRearmsSplash();
  } catch (const std::exception& exception) {
    std::cerr << "Test failure: " << exception.what() << '\n';
    return 1;
  }

  std::cout << "Fantome IV tests: OK\n";
  return 0;
}
