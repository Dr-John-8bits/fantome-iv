#pragma once

#include "fantome/FantomeEngine.h"
#include "fantome/OledView.h"
#include "fantome/UiState.h"

namespace fantome {

class StartupDisplayController {
 public:
  explicit StartupDisplayController(float splash_duration_s = 1.5f);

  void Reset();
  void Advance(float delta_seconds);
  bool ShowingSplash() const;
  float SplashDurationSeconds() const;
  float ElapsedSeconds() const;
  OledTextFrame Render(
    const OledTextRenderer& renderer,
    const UiState& ui,
    const FantomeEngine& engine) const;

 private:
  float splash_duration_s_ = 1.5f;
  float elapsed_seconds_ = 0.0f;
};

}  // namespace fantome
