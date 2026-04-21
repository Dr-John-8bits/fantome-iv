#include "fantome/OledView.h"

#include <algorithm>
#include <sstream>

namespace fantome {

namespace {

std::string PresetSlotLabel(std::size_t slot)
{
  return "P" + std::to_string(slot + 1);
}

}  // namespace

std::string OledTextFrame::ToDebugString() const
{
  std::ostringstream stream;
  for (std::size_t index = 0; index < rows.size(); ++index) {
    stream << rows[index];
    if (index + 1 < rows.size()) {
      stream << '\n';
    }
  }
  return stream.str();
}

OledTextFrame OledTextRenderer::RenderStartupSplash() const
{
  OledTextFrame frame;
  for (auto& row : frame.rows) {
    row = std::string(kDisplayColumns, ' ');
  }

  frame.rows[2] = CenterText("FANTOME IV", kDisplayColumns);
  frame.rows[3] = CenterText("by Dr. John.", kDisplayColumns);
  return frame;
}

OledTextFrame OledTextRenderer::Render(
  const UiState& ui,
  const FantomeEngine& engine) const
{
  const auto model = ui.BuildDisplayModel(engine);

  OledTextFrame frame;
  frame.rows[0] = PadRight(
    Clip(PresetSlotLabel(model.active_preset_slot) + " " + model.preset_name, kDisplayColumns),
    kDisplayColumns);
  frame.rows[1] = ComposeHeader(model.page_label, InteractionLabel(model.interaction_state), 15);

  const auto first_visible_index = FirstVisibleIndex(model);
  for (std::size_t row = 0; row < 5; ++row) {
    const auto item_index = first_visible_index + row;
    if (item_index >= model.item_count) {
      frame.rows[2 + row] = std::string(kDisplayColumns, ' ');
      continue;
    }

    const auto& item = model.items[item_index];
    char marker = ' ';
    if (item.selected) {
      marker = model.interaction_state == UiInteractionState::Confirmation && item.is_action ? '!' : '>';
    }

    frame.rows[2 + row] = ComposeItemRow(marker, item.label, item.value);
  }

  frame.rows[7] = PadRight(Clip(FooterText(model), kDisplayColumns), kDisplayColumns);
  return frame;
}

std::size_t OledTextRenderer::FirstVisibleIndex(const UiDisplayModel& model)
{
  if (model.item_count <= 5) {
    return 0;
  }

  std::size_t selected_index = 0;
  for (std::size_t index = 0; index < model.item_count; ++index) {
    if (model.items[index].selected) {
      selected_index = index;
      break;
    }
  }

  if (selected_index <= 2) {
    return 0;
  }

  const auto max_first = model.item_count - 5;
  return std::min(selected_index - 2, max_first);
}

std::string OledTextRenderer::InteractionLabel(UiInteractionState state)
{
  switch (state) {
    case UiInteractionState::Navigation:
      return "NAV";
    case UiInteractionState::Editing:
      return "EDIT";
    case UiInteractionState::Confirmation:
      return "CONF";
  }

  return "UNK";
}

std::string OledTextRenderer::CenterText(const std::string& text, std::size_t width)
{
  const auto clipped = Clip(text, width);
  if (clipped.size() >= width) {
    return clipped.substr(0, width);
  }

  const auto left_padding = (width - clipped.size()) / 2;
  const auto right_padding = width - clipped.size() - left_padding;
  return std::string(left_padding, ' ') + clipped + std::string(right_padding, ' ');
}

std::string OledTextRenderer::Clip(const std::string& text, std::size_t width)
{
  if (text.size() <= width) {
    return text;
  }

  if (width == 0) {
    return "";
  }

  if (width == 1) {
    return text.substr(0, 1);
  }

  return text.substr(0, width - 1) + "~";
}

std::string OledTextRenderer::PadRight(const std::string& text, std::size_t width)
{
  if (text.size() >= width) {
    return text.substr(0, width);
  }

  return text + std::string(width - text.size(), ' ');
}

std::string OledTextRenderer::ComposeHeader(
  const std::string& left,
  const std::string& right,
  std::size_t left_width)
{
  return PadRight(Clip(left, left_width), left_width) +
         PadRight(Clip(right, kDisplayColumns - left_width), kDisplayColumns - left_width);
}

std::string OledTextRenderer::ComposeItemRow(
  char marker,
  const std::string& label,
  const std::string& value)
{
  static constexpr std::size_t kLabelWidth = 12;
  static constexpr std::size_t kValueWidth = 8;

  return std::string(1, marker) +
         PadRight(Clip(label, kLabelWidth), kLabelWidth) +
         PadRight(Clip(value, kValueWidth), kValueWidth);
}

std::string OledTextRenderer::FooterText(const UiDisplayModel& model)
{
  if (model.interaction_state == UiInteractionState::Editing) {
    return "Turn=edit Enc=done";
  }

  if (model.interaction_state == UiInteractionState::Confirmation) {
    return "Enc=ok Back=cancel";
  }

  if (model.context_pot_needs_pickup) {
    return "Pickup K8 " + model.context_pot_label;
  }

  if (model.any_pot_needs_pickup) {
    return "Pickup active";
  }

  return model.status_text;
}

}  // namespace fantome
