#include "fantome/StartupDisplay.h"

#include <algorithm>

namespace fantome {

StartupDisplayController::StartupDisplayController(float splash_duration_s)
  : splash_duration_s_(std::max(0.0f, splash_duration_s))
{
}

void StartupDisplayController::Reset()
{
  elapsed_seconds_ = 0.0f;
}

void StartupDisplayController::Advance(float delta_seconds)
{
  elapsed_seconds_ = std::max(0.0f, elapsed_seconds_ + delta_seconds);
}

bool StartupDisplayController::ShowingSplash() const
{
  return elapsed_seconds_ < splash_duration_s_;
}

float StartupDisplayController::SplashDurationSeconds() const
{
  return splash_duration_s_;
}

float StartupDisplayController::ElapsedSeconds() const
{
  return elapsed_seconds_;
}

OledTextFrame StartupDisplayController::Render(
  const OledTextRenderer& renderer,
  const UiState& ui,
  const FantomeEngine& engine,
  const SessionManagerState* session_state) const
{
  if (ShowingSplash()) {
    return renderer.RenderStartupSplash();
  }

  return renderer.Render(ui, engine, session_state);
}

}  // namespace fantome
