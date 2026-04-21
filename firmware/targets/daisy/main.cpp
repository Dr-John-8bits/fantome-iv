#include <iostream>

#include "fantome/FirmwareRuntime.h"

#if __has_include("daisy_seed.h")
#include "daisy_seed.h"

int main()
{
  daisy::DaisySeed seed;
  seed.Configure();
  seed.Init();

  fantome::FirmwareRuntime runtime;
  runtime.BootStandalone(seed.AudioSampleRate());

  // Cible Daisy volontairement minimale :
  // l'audio callback, le scan des contrôles, l'OLED et le MIDI UART
  // seront branchés ici quand le hardware sera disponible.
  while (true) {
    runtime.AdvanceDisplay(0.001f);
  }

  return 0;
}

#else

int main()
{
  std::cout << "Fantome IV Daisy target stub (libDaisy unavailable)\n";

  fantome::FirmwareRuntime runtime;
  runtime.BootStandalone(48000.0f);
  runtime.AdvanceDisplay(1.6f);

  const auto output = runtime.BuildHardwareOutputFrame();
  std::cout << output.oled.ToDebugString() << '\n';
  return 0;
}

#endif
