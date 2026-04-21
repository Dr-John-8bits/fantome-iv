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

bool DaisyPlatformStub::Poll(HardwareInputFrame& input)
{
  input.ClearTransient();
  return false;
}

void DaisyPlatformStub::Present(const HardwareOutputFrame& output)
{
  last_output_ = output;
}

const HardwareOutputFrame& DaisyPlatformStub::LastOutput() const
{
  return last_output_;
}

}  // namespace fantome
