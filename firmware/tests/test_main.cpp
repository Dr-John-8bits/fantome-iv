#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "fantome/FantomeEngine.h"
#include "fantome/UiState.h"

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
    TestUiEncoderEditsFilterCutoff();
    TestSoftTakeoverBlocksUntilPickup();
    TestPresetPersistenceRoundTrip();
  } catch (const std::exception& exception) {
    std::cerr << "Test failure: " << exception.what() << '\n';
    return 1;
  }

  std::cout << "Fantome IV tests: OK\n";
  return 0;
}
