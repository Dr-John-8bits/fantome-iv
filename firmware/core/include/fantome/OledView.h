#pragma once

#include <array>
#include <cstddef>
#include <string>

#include "fantome/FantomeEngine.h"
#include "fantome/UiState.h"

namespace fantome {

struct SessionManagerState;

struct OledTextFrame {
  static constexpr std::size_t kRowCount = 8;

  std::array<std::string, kRowCount> rows {};

  std::string ToDebugString() const;
};

class OledTextRenderer {
 public:
  OledTextFrame RenderStartupSplash() const;
  OledTextFrame Render(
    const UiState& ui,
    const FantomeEngine& engine,
    const SessionManagerState* session_state = nullptr) const;

 private:
  static constexpr std::size_t kDisplayColumns = 21;

  static bool IsPresetBrowserFocused(const UiState& ui);
  static std::size_t FirstVisibleIndex(const UiDisplayModel& model);
  static std::string InteractionLabel(UiInteractionState state);
  static std::string CenterText(const std::string& text, std::size_t width);
  static std::string Clip(const std::string& text, std::size_t width);
  static std::string PadRight(const std::string& text, std::size_t width);
  static std::string ComposeHeader(
    const std::string& left,
    const std::string& right,
    std::size_t left_width);
  static std::string ComposeItemRow(
    char marker,
    const std::string& label,
    const std::string& value);
  static std::string ComposePresetBrowserRow(
    char marker,
    std::size_t slot,
    const std::string& name);
  static std::string BuildSystemSelectionSummary(const UiDisplayModel& model);
  static std::string SessionStatusText(const SessionManagerState* session_state);
  static std::string FooterText(
    const UiDisplayModel& model,
    const SessionManagerState* session_state);
};

}  // namespace fantome
