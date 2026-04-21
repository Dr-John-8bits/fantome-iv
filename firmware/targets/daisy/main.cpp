#include <iostream>

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

  bool Poll(HardwareInputFrame& input) override
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
    app.Tick(0.001f);
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
  app.Tick(1.6f);

  std::cout << platform.LastOutput().oled.ToDebugString() << '\n';
  return 0;
}

#endif
