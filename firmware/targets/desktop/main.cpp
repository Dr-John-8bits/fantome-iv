#include <filesystem>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "fantome/FirmwareRuntime.h"

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

  fantome::FirmwareRuntime runtime;
  const auto boot = runtime.BootWithSession(session_path.string(), 48000.0f);
  auto& engine = runtime.Engine();

  std::cout << "Fantome IV desktop smoke\n";
  std::cout << "startup (t=0.0s):\n"
            << runtime.BuildHardwareOutputFrame().oled.ToDebugString()
            << '\n';
  std::cout << "session: " << boot.session.message << '\n';
  std::cout << "patch: " << engine.CurrentPatch().name
            << " | mode: " << PlayModeToString(engine.CurrentPatch().play_mode)
            << " | midi ch: " << static_cast<int>(engine.CurrentPatch().midi_channel)
            << '\n';

  fantome::HardwareInputFrame input_frame;
  input_frame.PushMidi(fantome::MidiMessage::ControlChange(1, 74, 96));
  input_frame.PushMidi(fantome::MidiMessage::ControlChange(1, 71, 48));
  input_frame.PushMidi(fantome::MidiMessage::NoteOn(1, 60, 100));
  input_frame.PushMidi(fantome::MidiMessage::NoteOn(1, 64, 100));
  input_frame.PushMidi(fantome::MidiMessage::NoteOn(1, 67, 100));
  input_frame.PushMidi(fantome::MidiMessage::NoteOn(1, 71, 100));
  input_frame.PushMidi(fantome::MidiMessage::NoteOn(1, 74, 100));
  input_frame.PushMidi(fantome::MidiMessage::ControlChange(1, 1, 80));
  input_frame.PushMidi(fantome::MidiMessage::Start());
  runtime.ApplyHardwareFrame(input_frame);
  input_frame.ClearTransient();

  engine.SetClockTempoBpm(126.0f);
  for (int index = 0; index < 24; ++index) {
    runtime.HandleMidi(fantome::MidiMessage::Clock());
  }

  input_frame.buttons[static_cast<std::size_t>(fantome::HardwareButtonId::PageNext)].just_pressed = true;
  runtime.ApplyHardwareFrame(input_frame);
  input_frame.ClearTransient();
  input_frame.buttons[static_cast<std::size_t>(fantome::HardwareButtonId::Encoder)].just_pressed = true;
  runtime.ApplyHardwareFrame(input_frame);
  input_frame.ClearTransient();
  input_frame.encoder_delta = 2;
  runtime.ApplyHardwareFrame(input_frame);
  input_frame.ClearTransient();
  input_frame.pots[2] = {true, 0.38f};
  runtime.ApplyHardwareFrame(input_frame);
  input_frame.ClearTransient();
  input_frame.pots[2] = {false, 0.0f};
  for (int index = 0; index < 5; ++index) {
    input_frame.buttons[static_cast<std::size_t>(fantome::HardwareButtonId::PageNext)].just_pressed = true;
    runtime.ApplyHardwareFrame(input_frame);
    input_frame.ClearTransient();
  }
  input_frame.encoder_delta = 3;
  runtime.ApplyHardwareFrame(input_frame);
  input_frame.ClearTransient();

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
  runtime.Render(left.data(), right.data(), left.size());

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
  runtime.AdvanceDisplay(1.6f);
  const auto output = runtime.BuildHardwareOutputFrame();
  std::cout << "startup_active="
            << (output.startup_active ? "yes" : "no")
            << " elapsed=" << runtime.StartupDisplay().ElapsedSeconds()
            << " splash_dur=" << runtime.StartupDisplay().SplashDurationSeconds() << '\n';
  std::cout << "input: encoder=" << runtime.Controls().InputSurface().EncoderPosition()
            << " pot2=" << runtime.Controls().InputSurface().PotPositions()[2] << '\n';
  std::cout << "oled:\n"
            << output.oled.ToDebugString()
            << '\n';
  const auto shutdown_saved = runtime.Shutdown();
  std::cout << "shutdown_saved=" << (shutdown_saved ? "yes" : "no") << '\n';
  std::filesystem::remove(session_path);
  return 0;
}
