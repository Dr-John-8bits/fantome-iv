#include "DaisyApp.h"

namespace fantome {

DaisyApp::DaisyApp(DaisyPlatform& platform)
  : platform_(platform)
{
}

RuntimeBootResult DaisyApp::BootStandalone()
{
  platform_.Init();
  scanner_.Reset();
  runtime_.BootStandalone(platform_.AudioConfig().sample_rate);

  RuntimeBootResult result;
  result.standalone = true;
  return result;
}

RuntimeBootResult DaisyApp::BootWithSession(const std::string& session_path)
{
  platform_.Init();
  scanner_.Reset();
  return runtime_.BootWithSession(session_path, platform_.AudioConfig().sample_rate);
}

bool DaisyApp::TickControlFrame(float delta_seconds)
{
  RawHardwareInputFrame raw_input;
  const auto had_controls = platform_.Adc().Poll(raw_input);
  const auto had_midi = platform_.MidiUart().Drain(raw_input);
  HardwareInputFrame cooked_input;
  const auto scanned_input = scanner_.Scan(raw_input, cooked_input);
  const auto changed = runtime_.ApplyHardwareFrame(cooked_input);
  runtime_.AdvanceDisplay(delta_seconds);
  const auto output = runtime_.BuildHardwareOutputFrame();
  platform_.Oled().Present(output.oled);
  platform_.PresentIndicators(output);
  return had_controls || had_midi || scanned_input || changed;
}

bool DaisyApp::Tick(float delta_seconds)
{
  return TickControlFrame(delta_seconds);
}

void DaisyApp::RenderAudioBlock(HardwareAudioBuffer& buffer)
{
  runtime_.Render(buffer);
  const auto output = runtime_.BuildHardwareOutputFrame();
  platform_.Oled().Present(output.oled);
  platform_.PresentIndicators(output);
}

FirmwareRuntime& DaisyApp::Runtime()
{
  return runtime_;
}

const FirmwareRuntime& DaisyApp::Runtime() const
{
  return runtime_;
}

}  // namespace fantome
