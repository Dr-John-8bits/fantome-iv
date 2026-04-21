#pragma once

#include "fantome/FirmwareRuntime.h"

namespace fantome {

class DaisyPlatform {
 public:
  virtual ~DaisyPlatform() = default;

  virtual void Init() = 0;
  virtual float SampleRate() const = 0;
  virtual bool Poll(HardwareInputFrame& input) = 0;
  virtual void Present(const HardwareOutputFrame& output) = 0;
};

}  // namespace fantome
