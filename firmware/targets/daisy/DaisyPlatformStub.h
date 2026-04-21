#pragma once

#include "DaisyPlatform.h"

namespace fantome {

class DaisyPlatformStub final : public DaisyPlatform {
 public:
  void Init() override;
  float SampleRate() const override;
  bool Poll(HardwareInputFrame& input) override;
  void Present(const HardwareOutputFrame& output) override;

  const HardwareOutputFrame& LastOutput() const;

 private:
  float sample_rate_ = 48000.0f;
  HardwareOutputFrame last_output_ {};
};

}  // namespace fantome
