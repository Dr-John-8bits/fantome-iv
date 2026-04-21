#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

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

}  // namespace

int main()
{
  try {
    TestOldestVoiceSteal();
    TestProgramChangeLoadsPreset();
    TestCcMapping();
    TestSustainBehavior();
    TestMonoFallback();
  } catch (const std::exception& exception) {
    std::cerr << "Test failure: " << exception.what() << '\n';
    return 1;
  }

  std::cout << "Fantome IV tests: OK\n";
  return 0;
}

