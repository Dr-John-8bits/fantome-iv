#include <filesystem>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "fantome/FantomeEngine.h"
#include "fantome/OledView.h"
#include "fantome/PortableInput.h"
#include "fantome/SessionManager.h"
#include "fantome/StartupDisplay.h"
#include "fantome/UiState.h"

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
  const auto session_path =
    std::filesystem::temp_directory_path() / "fantome_iv_desktop_smoke_session.txt";
  fantome::FantomeEngine engine;
  fantome::UiState ui;
  fantome::OledTextRenderer oled;
  fantome::PortableInputSurface input;
  fantome::SessionManager session_manager;
  fantome::StartupDisplayController startup_display;
  engine.SetSampleRate(48000.0f);
  const auto boot = session_manager.Boot(session_path.string(), engine, ui);

  std::cout << "Fantome IV desktop smoke\n";
  std::cout << "startup (t=0.0s):\n"
            << startup_display.Render(oled, ui, engine, &session_manager.State()).ToDebugString()
            << '\n';
  std::cout << "session: " << boot.message << '\n';
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

  input.PressPageNext(engine, ui);
  input.PressEncoder(engine, ui);
  input.TurnEncoder(2, engine, ui);
  input.MovePot(2, 0.38f, engine, ui);
  for (int index = 0; index < 5; ++index) {
    input.PressPageNext(engine, ui);
  }
  input.TurnEncoder(3, engine, ui);

  std::cout << std::fixed << std::setprecision(2);
  std::cout << "cutoff=" << engine.CurrentPatch().filter.cutoff
            << " resonance=" << engine.CurrentPatch().filter.resonance
            << " modwheel=" << engine.Performance().mod_wheel
            << " tempo=" << engine.Transport().tempo_bpm
            << " clock_pulses=" << engine.Transport().midi_clock_pulses
            << '\n';

  PrintVoices(engine);

  std::vector<float> left(4096, 0.0f);
  std::vector<float> right(4096, 0.0f);
  engine.Render(left.data(), right.data(), left.size());

  const auto energy = std::inner_product(
    left.begin(),
    left.end(),
    right.begin(),
    0.0f,
    std::plus<float> {},
    [](float l, float r) {
      return (l * l) + (r * r);
    });

  std::cout << "render_energy=" << std::setprecision(6) << energy << '\n';
  std::cout << std::setprecision(2);
  startup_display.Advance(1.6f);
  std::cout << "startup_active="
            << (startup_display.ShowingSplash() ? "yes" : "no")
            << " elapsed=" << startup_display.ElapsedSeconds()
            << " splash_dur=" << startup_display.SplashDurationSeconds() << '\n';
  std::cout << "input: encoder=" << input.EncoderPosition()
            << " pot2=" << input.PotPositions()[2] << '\n';
  std::cout << "oled:\n"
            << startup_display.Render(oled, ui, engine, &session_manager.State()).ToDebugString()
            << '\n';
  const auto shutdown_saved = session_manager.Shutdown(engine, ui);
  std::cout << "shutdown_saved=" << (shutdown_saved ? "yes" : "no") << '\n';
  std::filesystem::remove(session_path);
  return 0;
}
