#pragma once

#include <vector>

#include "DaisyPlatform.h"

namespace fantome {

class DaisyPlatformStub final : public DaisyPlatform {
 public:
  void Init() override;
  float SampleRate() const override;
  bool Poll(RawHardwareInputFrame& input) override;
  void Present(const HardwareOutputFrame& output) override;

  void QueueInput(const RawHardwareInputFrame& input);

  const HardwareOutputFrame& LastOutput() const;

 private:
  float sample_rate_ = 48000.0f;
  HardwareOutputFrame last_output_ {};
  std::vector<RawHardwareInputFrame> pending_inputs_ {};
};

}  // namespace fantome
