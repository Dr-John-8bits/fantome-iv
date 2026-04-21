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
  runtime_.BootStandalone(platform_.SampleRate());

  RuntimeBootResult result;
  result.standalone = true;
  return result;
}

RuntimeBootResult DaisyApp::BootWithSession(const std::string& session_path)
{
  platform_.Init();
  scanner_.Reset();
  return runtime_.BootWithSession(session_path, platform_.SampleRate());
}

bool DaisyApp::TickControlFrame(float delta_seconds)
{
  RawHardwareInputFrame raw_input;
  const auto had_input = platform_.Poll(raw_input);
  HardwareInputFrame cooked_input;
  const auto scanned_input = scanner_.Scan(raw_input, cooked_input);
  const auto changed = runtime_.ApplyHardwareFrame(cooked_input);
  runtime_.AdvanceDisplay(delta_seconds);
  platform_.Present(runtime_.BuildHardwareOutputFrame());
  return had_input || scanned_input || changed;
}

bool DaisyApp::Tick(float delta_seconds)
{
  return TickControlFrame(delta_seconds);
}

void DaisyApp::RenderAudioBlock(HardwareAudioBuffer& buffer)
{
  runtime_.Render(buffer);
  platform_.Present(runtime_.BuildHardwareOutputFrame());
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
