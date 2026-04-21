#pragma once

#include <string>

#include "DaisyPlatform.h"

namespace fantome {

class DaisyApp {
 public:
  explicit DaisyApp(DaisyPlatform& platform);

  RuntimeBootResult BootStandalone();
  RuntimeBootResult BootWithSession(const std::string& session_path);
  bool Tick(float delta_seconds);

  FirmwareRuntime& Runtime();
  const FirmwareRuntime& Runtime() const;

 private:
  DaisyPlatform& platform_;
  FirmwareRuntime runtime_ {};
};

}  // namespace fantome
