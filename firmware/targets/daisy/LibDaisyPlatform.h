#pragma once

#include "DaisyPlatform.h"

#if __has_include("daisy_seed.h")
#define FANTOME_HAS_LIBDAISY 1
#include "daisy_seed.h"
#else
#define FANTOME_HAS_LIBDAISY 0
#endif

namespace fantome {

#if FANTOME_HAS_LIBDAISY

class LibDaisyPlatform final : public DaisyPlatform {
 public:
  void Init() override;
  DaisyAudioConfig AudioConfig() const override;
  DaisyControlAdc& Adc() override;
  DaisyMidiUart& MidiUart() override;
  DaisyOledDisplay& Oled() override;
  void PresentIndicators(const HardwareOutputFrame& output) override;
  const DaisyTargetConfig& TargetConfig() const override;
  DaisySessionStore* SessionStore() override;

  daisy::DaisySeed& Seed();
  const HardwareOutputFrame& LastOutput() const;

 private:
  class NullAdc final : public DaisyControlAdc {
   public:
    void Init() override {}
    bool Poll(RawHardwareInputFrame&) override { return false; }
  };

  class NullMidiUart final : public DaisyMidiUart {
   public:
    void Init() override {}
    bool Drain(RawHardwareInputFrame&) override { return false; }
  };

  class NullOled final : public DaisyOledDisplay {
   public:
    void Init() override {}
    void Present(const OledTextFrame&) override {}
  };

  daisy::DaisySeed seed_ {};
  DaisyAudioConfig audio_config_ {};
  DaisyTargetConfig target_config_ {DefaultDaisyTargetConfig()};
  DaisySessionFileStoreStub session_store_ {};
  NullAdc adc_ {};
  NullMidiUart midi_uart_ {};
  NullOled oled_ {};
  HardwareOutputFrame last_output_ {};
};

#endif

}  // namespace fantome
