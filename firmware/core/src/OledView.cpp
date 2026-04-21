#include "fantome/OledView.h"

#include <algorithm>
#include <sstream>

#include "fantome/SessionManager.h"

namespace fantome {

namespace {

std::string PresetSlotLabel(std::size_t slot)
{
  return "P" + std::to_string(slot + 1);
}

std::string ActiveSlotHeader(
  std::size_t slot,
  const std::string& patch_name,
  bool dirty)
{
  return PresetSlotLabel(slot) + (dirty ? "* " : " ") + patch_name;
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
  const FantomeEngine& engine,
  const SessionManagerState* session_state) const
{
  const auto model = ui.BuildDisplayModel(engine);

  OledTextFrame frame;
  frame.rows[0] = PadRight(
    Clip(
      ActiveSlotHeader(
        model.active_preset_slot,
        model.preset_name,
        model.current_patch_dirty),
      kDisplayColumns),
    kDisplayColumns);
  frame.rows[1] = ComposeHeader(
    ui.CurrentPage() == UiPage::System ? "User Slots" : model.page_label,
    InteractionLabel(model.interaction_state),
    15);

  if (ui.CurrentPage() == UiPage::System) {
    for (std::size_t slot = 0; slot < kPresetCount; ++slot) {
      char marker = ' ';
      if (IsPresetBrowserFocused(ui) && slot == model.target_preset_slot) {
        marker = '>';
      } else if (slot == model.active_preset_slot) {
        marker = '*';
      } else if (slot == model.target_preset_slot) {
        marker = '+';
      }

      frame.rows[2 + slot] =
        ComposePresetBrowserRow(marker, slot, engine.PresetBank()[slot].name);
    }

    frame.rows[6] = PadRight(
      Clip(BuildSystemSelectionSummary(model), kDisplayColumns),
      kDisplayColumns);
    frame.rows[7] = PadRight(
      Clip(FooterText(model, session_state), kDisplayColumns),
      kDisplayColumns);
    return frame;
  }

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

  frame.rows[7] = PadRight(Clip(FooterText(model, session_state), kDisplayColumns), kDisplayColumns);
  return frame;
}

bool OledTextRenderer::IsPresetBrowserFocused(const UiState& ui)
{
  return ui.CurrentPage() == UiPage::System && ui.SelectedItemIndex() >= 3;
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

std::string OledTextRenderer::ComposePresetBrowserRow(
  char marker,
  std::size_t slot,
  const std::string& name)
{
  static constexpr std::size_t kNameWidth = 17;
  return std::string(1, marker) +
         PadRight(PresetSlotLabel(slot) + " ", 3) +
         PadRight(Clip(name, kNameWidth), kNameWidth);
}

std::string OledTextRenderer::BuildSystemSelectionSummary(const UiDisplayModel& model)
{
  std::string summary;
  if (model.selected_value.empty()) {
    summary = "Sel:" + model.selected_label;
  } else {
    summary = "Sel:" + model.selected_label + " " + model.selected_value;
  }

  if (model.current_patch_dirty) {
    summary += " *";
  }

  return summary;
}

std::string OledTextRenderer::SessionStatusText(const SessionManagerState* session_state)
{
  if (session_state == nullptr) {
    return "Sess:standalone";
  }

  if (!session_state->last_error.empty()) {
    return "Sess:error";
  }

  std::string boot_mode = "fresh";
  switch (session_state->last_boot_mode) {
    case SessionBootMode::FreshStart:
      boot_mode = "fresh";
      break;
    case SessionBootMode::RestoredFromDisk:
      boot_mode = "restored";
      break;
    case SessionBootMode::FallbackFreshStart:
      boot_mode = "fallback";
      break;
  }

  return "Sess:" + boot_mode + (session_state->active ? " on" : " off");
}

std::string OledTextRenderer::FooterText(
  const UiDisplayModel& model,
  const SessionManagerState* session_state)
{
  if (model.interaction_state == UiInteractionState::Editing) {
    return "Turn=edit Enc=done";
  }

  if (model.interaction_state == UiInteractionState::Confirmation) {
    return "Enc=ok Back=cancel";
  }

  if (model.page_label == "System") {
    return SessionStatusText(session_state);
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
