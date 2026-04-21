#include <iostream>
#include <vector>

#include "DaisyApp.h"
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
  }

  float SampleRate() const override
  {
    return seed_.AudioSampleRate();
  }

  bool Poll(RawHardwareInputFrame& input) override
  {
    input.ClearTransient();
    return false;
  }

  void Present(const HardwareOutputFrame& output) override
  {
    last_output_ = output;
  }

 private:
  daisy::DaisySeed seed_ {};
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
  raw_input.PushMidi(fantome::MidiMessage::NoteOn(1, 60, 110));
  raw_input.pot_available[1] = true;
  raw_input.pots[1] = 0.66f;
  platform.QueueInput(raw_input);
  app.TickControlFrame(0.02f);

  std::vector<float> left(512, 0.0f);
  std::vector<float> right(512, 0.0f);
  fantome::HardwareAudioBuffer buffer {left.data(), right.data(), left.size()};
  app.RenderAudioBlock(buffer);
  std::cout << "audio_blocks=" << platform.LastOutput().audio_block_count
            << " peak=" << platform.LastOutput().output_peak
            << " clip=" << (platform.LastOutput().output_clip ? "yes" : "no")
            << '\n';
  app.TickControlFrame(1.6f);

  std::cout << platform.LastOutput().oled.ToDebugString() << '\n';
  return 0;
}

#endif
