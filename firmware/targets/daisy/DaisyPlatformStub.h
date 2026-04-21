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
  const DaisyTargetConfig& TargetConfig() const override;
  DaisySessionStore* SessionStore() override;

  void SetAudioConfig(const DaisyAudioConfig& config);
  void SetSessionPath(const std::string& session_path);
  const HardwareOutputFrame& LastOutput() const;
  DaisyControlAdcStub& AdcStub();
  DaisyMidiUartStub& MidiStub();
  DaisyOledDisplayStub& OledStub();
  DaisySessionFileStoreStub& SessionStoreStub();

 private:
  DaisyAudioConfig audio_config_ {DefaultDaisyTargetConfig().audio};
  DaisyTargetConfig target_config_ {DefaultDaisyTargetConfig()};
  HardwareOutputFrame last_output_ {};
  DaisySessionFileStoreStub session_store_ {};
  DaisyControlAdcStub adc_ {};
  DaisyMidiUartStub midi_uart_ {};
  DaisyOledDisplayStub oled_ {};
};

}  // namespace fantome
