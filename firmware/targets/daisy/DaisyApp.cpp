#include "DaisyApp.h"

namespace fantome {

DaisyApp::DaisyApp(DaisyPlatform& platform)
  : platform_(platform)
{
}

RuntimeBootResult DaisyApp::BootFromPlatform()
{
  if (auto* store = platform_.SessionStore()) {
    return BootWithSessionStore(*store);
  }

  return BootStandalone();
}

RuntimeBootResult DaisyApp::BootStandalone()
{
  platform_.Init();
  scanner_.Reset();
  session_store_ = nullptr;
  runtime_.BootStandalone(platform_.AudioConfig().sample_rate);

  RuntimeBootResult result;
  result.standalone = true;
  return result;
}

RuntimeBootResult DaisyApp::BootWithSession(const std::string& session_path)
{
  platform_.Init();
  scanner_.Reset();
  session_store_ = nullptr;
  return runtime_.BootWithSession(session_path, platform_.AudioConfig().sample_rate);
}

RuntimeBootResult DaisyApp::BootWithSessionStore(DaisySessionStore& session_store)
{
  platform_.Init();
  scanner_.Reset();
  session_store.Init();
  session_store_ = &session_store;
  return runtime_.BootWithSession(session_store.SessionPath(), platform_.AudioConfig().sample_rate);
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

void DaisyApp::RenderInterleavedAudio(float* interleaved, std::size_t frame_count)
{
  if (interleaved == nullptr || frame_count == 0) {
    return;
  }

  scratch_left_.assign(frame_count, 0.0f);
  scratch_right_.assign(frame_count, 0.0f);
  HardwareAudioBuffer buffer {scratch_left_.data(), scratch_right_.data(), frame_count};
  RenderAudioBlock(buffer);

  for (std::size_t index = 0; index < frame_count; ++index) {
    interleaved[index * 2] = scratch_left_[index];
    interleaved[(index * 2) + 1] = scratch_right_[index];
  }
}

bool DaisyApp::SaveSessionCheckpointToStore()
{
  return session_store_ != nullptr && runtime_.SaveSessionCheckpoint();
}

bool DaisyApp::ReloadSessionFromStore()
{
  return session_store_ != nullptr && runtime_.ReloadSession();
}

bool DaisyApp::ShutdownToStore()
{
  if (session_store_ == nullptr) {
    return runtime_.Shutdown();
  }

  return runtime_.Shutdown();
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
