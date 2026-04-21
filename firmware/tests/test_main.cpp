#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "fantome/FantomeEngine.h"

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
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));

  std::vector<float> left(1024, 0.0f);
  std::vector<float> right(1024, 0.0f);
  engine.Render(left.data(), right.data(), left.size());

  Expect(BufferEnergy(left, right) > 0.001f,
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
  } catch (const std::exception& exception) {
    std::cerr << "Test failure: " << exception.what() << '\n';
    return 1;
  }

  std::cout << "Fantome IV tests: OK\n";
  return 0;
}
