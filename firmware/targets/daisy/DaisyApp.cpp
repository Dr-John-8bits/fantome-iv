#include "DaisyApp.h"

namespace fantome {

DaisyApp::DaisyApp(DaisyPlatform& platform)
  : platform_(platform)
{
}

RuntimeBootResult DaisyApp::BootStandalone()
{
  platform_.Init();
  runtime_.BootStandalone(platform_.SampleRate());

  RuntimeBootResult result;
  result.standalone = true;
  return result;
}

RuntimeBootResult DaisyApp::BootWithSession(const std::string& session_path)
{
  platform_.Init();
  return runtime_.BootWithSession(session_path, platform_.SampleRate());
}

bool DaisyApp::Tick(float delta_seconds)
{
  HardwareInputFrame input;
  const auto had_input = platform_.Poll(input);
  const auto changed = runtime_.ApplyHardwareFrame(input);
  runtime_.AdvanceDisplay(delta_seconds);
  platform_.Present(runtime_.BuildHardwareOutputFrame());
  return had_input || changed;
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
