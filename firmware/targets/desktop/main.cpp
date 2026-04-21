#include <iomanip>
#include <iostream>
#include <string>

#include "fantome/FantomeEngine.h"

namespace {

std::string PlayModeToString(fantome::PlayMode mode)
{
  switch (mode) {
    case fantome::PlayMode::Poly:
      return "Poly";
    case fantome::PlayMode::Mono:
      return "Mono";
    case fantome::PlayMode::Unison:
      return "Unison";
  }

  return "Unknown";
}

void PrintVoices(const fantome::FantomeEngine& engine)
{
  const auto& voices = engine.Voices();
  for (std::size_t index = 0; index < voices.size(); ++index) {
    const auto& voice = voices[index];
    std::cout << "voice[" << index << "] "
              << (voice.active ? "active" : "idle")
              << " note=" << static_cast<int>(voice.note)
              << " vel=" << static_cast<int>(voice.velocity)
              << " age=" << voice.age
              << '\n';
  }
}

}  // namespace

int main()
{
  fantome::FantomeEngine engine;

  std::cout << "Fantome IV desktop smoke\n";
  std::cout << "patch: " << engine.CurrentPatch().name
            << " | mode: " << PlayModeToString(engine.CurrentPatch().play_mode)
            << " | midi ch: " << static_cast<int>(engine.CurrentPatch().midi_channel)
            << '\n';

  engine.HandleMidi(fantome::MidiMessage::ControlChange(1, 74, 96));
  engine.HandleMidi(fantome::MidiMessage::ControlChange(1, 71, 48));
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 60, 100));
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 64, 100));
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 67, 100));
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 71, 100));
  engine.HandleMidi(fantome::MidiMessage::NoteOn(1, 74, 100));
  engine.HandleMidi(fantome::MidiMessage::ControlChange(1, 1, 80));
  engine.HandleMidi(fantome::MidiMessage::Start());
  engine.SetClockTempoBpm(126.0f);
  for (int index = 0; index < 24; ++index) {
    engine.HandleMidi(fantome::MidiMessage::Clock());
  }

  std::cout << std::fixed << std::setprecision(2);
  std::cout << "cutoff=" << engine.CurrentPatch().filter.cutoff
            << " resonance=" << engine.CurrentPatch().filter.resonance
            << " modwheel=" << engine.Performance().mod_wheel
            << " tempo=" << engine.Transport().tempo_bpm
            << " clock_pulses=" << engine.Transport().midi_clock_pulses
            << '\n';

  PrintVoices(engine);
  return 0;
}

