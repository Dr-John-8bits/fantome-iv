#include "DaisyPlatformStub.h"

namespace fantome {

void DaisyPlatformStub::Init()
{
  if (audio_config_.sample_rate <= 0.0f) {
    audio_config_.sample_rate = 48000.0f;
  }
  if (audio_config_.block_size == 0) {
    audio_config_.block_size = 48;
  }
  adc_.Init();
  midi_uart_.Init();
  oled_.Init();
  last_output_ = HardwareOutputFrame {};
}

DaisyAudioConfig DaisyPlatformStub::AudioConfig() const
{
  return audio_config_;
}

DaisyControlAdc& DaisyPlatformStub::Adc()
{
  return adc_;
}

DaisyMidiUart& DaisyPlatformStub::MidiUart()
{
  return midi_uart_;
}

DaisyOledDisplay& DaisyPlatformStub::Oled()
{
  return oled_;
}

void DaisyPlatformStub::PresentIndicators(const HardwareOutputFrame& output)
{
  last_output_ = output;
  last_output_.oled = oled_.LastFrame();
}

void DaisyPlatformStub::SetAudioConfig(const DaisyAudioConfig& config)
{
  audio_config_.sample_rate = config.sample_rate;
  audio_config_.block_size = config.block_size;
}

const HardwareOutputFrame& DaisyPlatformStub::LastOutput() const
{
  return last_output_;
}

DaisyControlAdcStub& DaisyPlatformStub::AdcStub()
{
  return adc_;
}

DaisyMidiUartStub& DaisyPlatformStub::MidiStub()
{
  return midi_uart_;
}

DaisyOledDisplayStub& DaisyPlatformStub::OledStub()
{
  return oled_;
}

}  // namespace fantome
