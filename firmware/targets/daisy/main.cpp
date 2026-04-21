#include <iostream>
#include <vector>

#include "DaisyApp.h"
#include "DaisyPeripherals.h"
#include "DaisyPlatformStub.h"

#if __has_include("daisy_seed.h")
#include "daisy_seed.h"

namespace fantome {

class LibDaisyPlatform final : public DaisyPlatform {
 public:
  void Init() override
  {
    seed_.Configure();
    seed_.Init();
    audio_.sample_rate = seed_.AudioSampleRate();
    audio_.block_size = 48;
    adc_.Init();
    midi_uart_.Init();
    oled_.Init();
  }

  DaisyAudioConfig AudioConfig() const override
  {
    return audio_;
  }

  DaisyControlAdc& Adc() override
  {
    return adc_;
  }

  DaisyMidiUart& MidiUart() override
  {
    return midi_uart_;
  }

  DaisyOledDisplay& Oled() override
  {
    return oled_;
  }

  void PresentIndicators(const HardwareOutputFrame& output) override
  {
    last_output_ = output;
  }

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
  DaisyAudioConfig audio_ {};
  NullAdc adc_ {};
  NullMidiUart midi_uart_ {};
  NullOled oled_ {};
  HardwareOutputFrame last_output_ {};
};

}  // namespace fantome

int main()
{
  fantome::LibDaisyPlatform platform;
  fantome::DaisyApp app(platform);
  app.BootStandalone();

  // Intégration volontairement mince :
  // le vrai callback audio, le scan des contrôles, l'OLED et le MIDI UART
  // seront branchés ici en s'appuyant sur DaisyPlatform.
  while (true) {
    app.TickControlFrame(0.001f);
  }

  return 0;
}

#else

int main()
{
  std::cout << "Fantome IV Daisy target stub (libDaisy unavailable)\n";

  fantome::DaisyPlatformStub platform;
  fantome::DaisyApp app(platform);
  app.BootStandalone();
  fantome::RawHardwareInputFrame raw_input;
  raw_input.pot_available[1] = true;
  raw_input.pots[1] = 0.66f;
  platform.AdcStub().QueueFrame(raw_input);
  platform.MidiStub().QueueMessage(fantome::MidiMessage::NoteOn(1, 60, 110));
  platform.MidiStub().QueueMessage(fantome::MidiMessage::ControlChange(1, 74, 84));
  app.TickControlFrame(0.02f);

  std::vector<float> left(512, 0.0f);
  std::vector<float> right(512, 0.0f);
  fantome::HardwareAudioBuffer buffer {left.data(), right.data(), left.size()};
  app.RenderAudioBlock(buffer);
  std::cout << "audio_blocks=" << platform.LastOutput().audio_block_count
            << " peak=" << platform.LastOutput().output_peak
            << " clip=" << (platform.LastOutput().output_clip ? "yes" : "no")
            << " oled_present=" << platform.OledStub().PresentCount()
            << '\n';
  app.TickControlFrame(1.6f);

  std::cout << platform.LastOutput().oled.ToDebugString() << '\n';
  return 0;
}

#endif
