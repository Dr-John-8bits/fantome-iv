#pragma once

#include "DaisyPlatform.h"

namespace fantome {

class DaisyPlatformStub final : public DaisyPlatform {
 public:
  void Init() override;
  DaisyAudioConfig AudioConfig() const override;
  DaisyControlAdc& Adc() override;
  DaisyMidiUart& MidiUart() override;
  DaisyOledDisplay& Oled() override;
  void PresentIndicators(const HardwareOutputFrame& output) override;

  void SetAudioConfig(const DaisyAudioConfig& config);
  const HardwareOutputFrame& LastOutput() const;
  DaisyControlAdcStub& AdcStub();
  DaisyMidiUartStub& MidiStub();
  DaisyOledDisplayStub& OledStub();

 private:
  DaisyAudioConfig audio_config_ {};
  HardwareOutputFrame last_output_ {};
  DaisyControlAdcStub adc_ {};
  DaisyMidiUartStub midi_uart_ {};
  DaisyOledDisplayStub oled_ {};
};

}  // namespace fantome
