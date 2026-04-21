#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "DaisyPlatform.h"
#include "DaisySessionStore.h"

namespace fantome {

class DaisyApp {
 public:
  explicit DaisyApp(DaisyPlatform& platform);

  RuntimeBootResult BootStandalone();
  RuntimeBootResult BootWithSession(const std::string& session_path);
  RuntimeBootResult BootWithSessionStore(DaisySessionStore& session_store);
  bool TickControlFrame(float delta_seconds);
  bool Tick(float delta_seconds);
  void RenderAudioBlock(HardwareAudioBuffer& buffer);
  void RenderInterleavedAudio(float* interleaved, std::size_t frame_count);
  bool SaveSessionCheckpointToStore();
  bool ReloadSessionFromStore();
  bool ShutdownToStore();

  FirmwareRuntime& Runtime();
  const FirmwareRuntime& Runtime() const;

 private:
  DaisyPlatform& platform_;
  DaisySessionStore* session_store_ = nullptr;
  HardwareControlScanner scanner_ {};
  FirmwareRuntime runtime_ {};
  std::vector<float> scratch_left_ {};
  std::vector<float> scratch_right_ {};
};

}  // namespace fantome
