#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "fantome/FantomeEngine.h"
#include "fantome/OledView.h"
#include "fantome/PortableInput.h"
#include "fantome/SessionManager.h"
#include "fantome/SessionPersistence.h"
#include "fantome/StartupDisplay.h"
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
  Expect(debug.find("Save Preset") != std::string::npos,
         "oled view should show the selected action label");
  Expect(debug.find("Enc=ok") != std::string::npos,
         "oled view should show confirmation guidance");
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
         "fresh boot should start from the factory first patch");
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
    TestUiEncoderEditsFilterCutoff();
    TestSoftTakeoverBlocksUntilPickup();
    TestPresetPersistenceRoundTrip();
    TestOledRendererShowsStartupSplash();
    TestOledRendererShowsPresetAndSelection();
    TestOledRendererShowsPickupState();
    TestOledRendererShowsConfirmationState();
    TestPortableSessionPersistenceRoundTrip();
    TestPortableInputSurfaceCanNavigateAndEdit();
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
