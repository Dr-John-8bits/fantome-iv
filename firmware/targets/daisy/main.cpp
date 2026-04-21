#include <iostream>
#include <vector>

#include "DaisyApp.h"
#include "DaisyTargetConfig.h"
#include "DaisyPeripherals.h"
#include "DaisyPlatformStub.h"
#include "LibDaisyPlatform.h"

#if FANTOME_HAS_LIBDAISY
int main()
{
  fantome::LibDaisyPlatform platform;
  fantome::DaisyApp app(platform);
  app.BootFromPlatform();

  // Intégration volontairement mince :
  // la couture cible est désormais préparée, avec pinmap figée, store de
  // session et adaptation audio interleavée côté app. Le branchement
  // effectif du callback libDaisy et des drivers réels reste la prochaine
  // validation à faire sur la vraie carte.
  const auto delta_seconds = static_cast<float>(platform.AudioConfig().block_size) /
                             platform.AudioConfig().sample_rate;
  while (true) {
    app.TickControlFrame(delta_seconds);
  }

  return 0;
}

#else

int main()
{
  std::cout << "Fantome IV Daisy target stub (libDaisy unavailable)\n";

  fantome::DaisyPlatformStub platform;
  fantome::DaisyApp app(platform);
  const auto boot = app.BootFromPlatform();
  platform.AdcStub().QueuePotRawSample(1, 2700);
  platform.MidiStub().QueueBytes({0x90, 60, 110, 0xB0, 74, 84});
  app.TickControlFrame(0.02f);

  std::vector<float> interleaved(512 * 2, 0.0f);
  app.RenderInterleavedAudio(interleaved.data(), 512);
  std::cout << "audio_blocks=" << platform.LastOutput().audio_block_count
            << " peak=" << platform.LastOutput().output_peak
            << " clip=" << (platform.LastOutput().output_clip ? "yes" : "no")
            << " oled_present=" << platform.OledStub().PresentCount()
            << " boot=" << (boot.standalone ? "standalone" : "session")
            << '\n';
  std::cout << "config=" << fantome::DaisyTargetConfigSummary(platform.TargetConfig()) << '\n';
  std::cout << "session_backend=" << platform.SessionStoreStub().BackendLabel()
            << " path=" << platform.SessionStoreStub().SessionPath() << '\n';
  app.TickControlFrame(1.6f);

  std::cout << platform.LastOutput().oled.ToDebugString() << '\n';
  return 0;
}

#endif
