#include "DaisyPlatformStub.h"

namespace fantome {

void DaisyPlatformStub::Init()
{
  sample_rate_ = 48000.0f;
}

float DaisyPlatformStub::SampleRate() const
{
  return sample_rate_;
}

bool DaisyPlatformStub::Poll(RawHardwareInputFrame& input)
{
  if (pending_inputs_.empty()) {
    input.ClearTransient();
    return false;
  }

  input = pending_inputs_.front();
  pending_inputs_.erase(pending_inputs_.begin());
  return true;
}

void DaisyPlatformStub::Present(const HardwareOutputFrame& output)
{
  last_output_ = output;
}

void DaisyPlatformStub::QueueInput(const RawHardwareInputFrame& input)
{
  pending_inputs_.push_back(input);
}

const HardwareOutputFrame& DaisyPlatformStub::LastOutput() const
{
  return last_output_;
}

}  // namespace fantome
