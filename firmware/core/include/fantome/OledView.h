#pragma once

#include <array>
#include <cstddef>
#include <string>

#include "fantome/FantomeEngine.h"
#include "fantome/UiState.h"

namespace fantome {

struct OledTextFrame {
  static constexpr std::size_t kRowCount = 8;

  std::array<std::string, kRowCount> rows {};

  std::string ToDebugString() const;
};

class OledTextRenderer {
 public:
  OledTextFrame RenderStartupSplash() const;
  OledTextFrame Render(const UiState& ui, const FantomeEngine& engine) const;

 private:
  static constexpr std::size_t kDisplayColumns = 21;

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
  static std::string FooterText(const UiDisplayModel& model);
};

}  // namespace fantome
