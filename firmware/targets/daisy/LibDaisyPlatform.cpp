#include "LibDaisyPlatform.h"

namespace fantome {

#if FANTOME_HAS_LIBDAISY

void LibDaisyPlatform::Init()
{
  seed_.Configure();
  seed_.Init();
  audio_config_ = target_config_.audio;
  if (seed_.AudioSampleRate() > 1000.0f) {
    audio_config_.sample_rate = seed_.AudioSampleRate();
  }

  adc_.Init();
  midi_uart_.Init();
  oled_.Init();
  last_output_ = HardwareOutputFrame {};
}

DaisyAudioConfig LibDaisyPlatform::AudioConfig() const
{
  return audio_config_;
}

DaisyControlAdc& LibDaisyPlatform::Adc()
{
  return adc_;
}

DaisyMidiUart& LibDaisyPlatform::MidiUart()
{
  return midi_uart_;
}

DaisyOledDisplay& LibDaisyPlatform::Oled()
{
  return oled_;
}

void LibDaisyPlatform::PresentIndicators(const HardwareOutputFrame& output)
{
  last_output_ = output;
}

const DaisyTargetConfig& LibDaisyPlatform::TargetConfig() const
{
  return target_config_;
}

DaisySessionStore* LibDaisyPlatform::SessionStore()
{
  return &session_store_;
}

daisy::DaisySeed& LibDaisyPlatform::Seed()
{
  return seed_;
}

const HardwareOutputFrame& LibDaisyPlatform::LastOutput() const
{
  return last_output_;
}

#endif

}  // namespace fantome
