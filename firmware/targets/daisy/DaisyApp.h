#pragma once

#include <string>

#include "DaisyPlatform.h"

namespace fantome {

class DaisyApp {
 public:
  explicit DaisyApp(DaisyPlatform& platform);

  RuntimeBootResult BootStandalone();
  RuntimeBootResult BootWithSession(const std::string& session_path);
  bool TickControlFrame(float delta_seconds);
  bool Tick(float delta_seconds);
  void RenderAudioBlock(HardwareAudioBuffer& buffer);

  FirmwareRuntime& Runtime();
  const FirmwareRuntime& Runtime() const;

 private:
  DaisyPlatform& platform_;
  HardwareControlScanner scanner_ {};
  FirmwareRuntime runtime_ {};
};

}  // namespace fantome
