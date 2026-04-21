#pragma once

#include "DaisyPeripherals.h"
#include "DaisySessionStore.h"
#include "DaisyTargetConfig.h"
#include "fantome/FirmwareRuntime.h"

namespace fantome {

class DaisyPlatform {
 public:
  virtual ~DaisyPlatform() = default;

  virtual void Init() = 0;
  virtual DaisyAudioConfig AudioConfig() const = 0;
  virtual DaisyControlAdc& Adc() = 0;
  virtual DaisyMidiUart& MidiUart() = 0;
  virtual DaisyOledDisplay& Oled() = 0;
  virtual void PresentIndicators(const HardwareOutputFrame& output) = 0;
  virtual const DaisyTargetConfig& TargetConfig() const = 0;
  virtual DaisySessionStore* SessionStore() { return nullptr; }
};

}  // namespace fantome
