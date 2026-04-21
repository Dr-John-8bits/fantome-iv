#pragma once

#include "DaisyPeripherals.h"
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
};

}  // namespace fantome
