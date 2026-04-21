#include "fantome/SessionPersistence.h"

#include <fstream>
#include <string>

#include "fantome/PatchSerialization.h"

namespace fantome {

namespace {

constexpr const char* kSessionHeader = "FANTOME_IV_SESSION_V1";

enum class ParseSection {
  None,
  Engine,
  CurrentPatch,
  Preset,
  Ui,
};

bool ParseBool(const std::string& value, bool& output)
{
  if (value == "0") {
    output = false;
    return true;
  }
  if (value == "1") {
    output = true;
    return true;
  }
  return false;
}

bool ParseInt(const std::string& value, int& output)
{
  try {
    output = std::stoi(value);
    return true;
  } catch (...) {
    return false;
  }
}

bool ParseSize(const std::string& value, std::size_t& output)
{
  try {
    output = static_cast<std::size_t>(std::stoul(value));
    return true;
  } catch (...) {
    return false;
  }
}

bool ParseFloat(const std::string& value, float& output)
{
  try {
    output = std::stof(value);
    return true;
  } catch (...) {
    return false;
  }
}

PortableSessionState MakeDefaultSessionState()
{
  PortableSessionState state;
  state.engine.preset_bank = MakeDefaultUserPresetBank();
  state.engine.current_patch = state.engine.preset_bank[0];
  state.engine.active_preset_slot = 0;
  state.engine.tempo_bpm = 120.0f;
  state.ui.preset_target_slot = 0;
  return state;
}

bool ApplyUiKeyValue(UiSessionState& ui, const std::string& key, const std::string& value)
{
  int int_value = 0;
  std::size_t size_value = 0;
  float float_value = 0.0f;
  bool bool_value = false;

  if (key == "current_page" && ParseInt(value, int_value)) {
    ui.current_page = static_cast<UiPage>(int_value);
    return true;
  }
  if (key == "interaction_state" && ParseInt(value, int_value)) {
    ui.interaction_state = static_cast<UiInteractionState>(int_value);
    return true;
  }
  if (key == "selected_item_index" && ParseSize(value, size_value)) {
    ui.selected_item_index = size_value;
    return true;
  }
  if (key == "preset_target_slot" && ParseSize(value, size_value)) {
    ui.preset_target_slot = size_value;
    return true;
  }
  if (key == "pending_action" && ParseInt(value, int_value)) {
    ui.pending_action = static_cast<UiAction>(int_value);
    return true;
  }

  for (std::size_t index = 0; index < ui.pots.size(); ++index) {
    const auto prefix = "pot" + std::to_string(index) + ".";
    if (key == prefix + "captured" && ParseBool(value, bool_value)) {
      ui.pots[index].captured = bool_value;
      return true;
    }
    if (key == prefix + "has_physical_value" && ParseBool(value, bool_value)) {
      ui.pots[index].has_physical_value = bool_value;
      return true;
    }
    if (key == prefix + "physical_value" && ParseFloat(value, float_value)) {
      ui.pots[index].physical_value = float_value;
      return true;
    }
  }

  return false;
}

void WriteUiState(std::ofstream& output, const UiSessionState& ui)
{
  output << "[ui]\n";
  output << "current_page=" << static_cast<int>(ui.current_page) << '\n';
  output << "interaction_state=" << static_cast<int>(ui.interaction_state) << '\n';
  output << "selected_item_index=" << ui.selected_item_index << '\n';
  output << "preset_target_slot=" << ui.preset_target_slot << '\n';
  output << "pending_action=" << static_cast<int>(ui.pending_action) << '\n';
  for (std::size_t index = 0; index < ui.pots.size(); ++index) {
    output << "pot" << index << ".captured=" << (ui.pots[index].captured ? 1 : 0) << '\n';
    output << "pot" << index << ".has_physical_value="
           << (ui.pots[index].has_physical_value ? 1 : 0) << '\n';
    output << "pot" << index << ".physical_value=" << ui.pots[index].physical_value << '\n';
  }
}

}  // namespace

bool SessionPersistence::Save(const std::string& path, const PortableSessionState& state)
{
  std::ofstream output(path, std::ios::trunc);
  if (!output.is_open()) {
    return false;
  }

  output << kSessionHeader << '\n';
  output << "[engine]\n";
  output << "active_preset_slot=" << state.engine.active_preset_slot << '\n';
  output << "tempo_bpm=" << state.engine.tempo_bpm << '\n';
  output << "[current_patch]\n";
  PatchSerialization::WriteKeyValues(output, state.engine.current_patch);
  for (std::size_t slot = 0; slot < state.engine.preset_bank.size(); ++slot) {
    output << "[preset " << slot << "]\n";
    PatchSerialization::WriteKeyValues(output, state.engine.preset_bank[slot]);
  }
  WriteUiState(output, state.ui);
  return output.good();
}

bool SessionPersistence::Load(const std::string& path, PortableSessionState& state)
{
  std::ifstream input(path);
  if (!input.is_open()) {
    return false;
  }

  std::string line;
  if (!std::getline(input, line) || line != kSessionHeader) {
    return false;
  }

  state = MakeDefaultSessionState();
  ParseSection section = ParseSection::None;
  std::size_t current_preset_index = 0;
  bool saw_engine = false;
  bool saw_current_patch = false;
  bool saw_ui = false;

  while (std::getline(input, line)) {
    if (line.empty()) {
      continue;
    }

    if (line == "[engine]") {
      section = ParseSection::Engine;
      saw_engine = true;
      continue;
    }
    if (line == "[current_patch]") {
      section = ParseSection::CurrentPatch;
      saw_current_patch = true;
      continue;
    }
    if (line == "[ui]") {
      section = ParseSection::Ui;
      saw_ui = true;
      continue;
    }
    if (line.rfind("[preset ", 0) == 0 && line.back() == ']') {
      const auto slot_text = line.substr(8, line.size() - 9);
      if (!ParseSize(slot_text, current_preset_index) ||
          current_preset_index >= state.engine.preset_bank.size()) {
        return false;
      }
      section = ParseSection::Preset;
      continue;
    }

    const auto separator = line.find('=');
    if (separator == std::string::npos) {
      return false;
    }

    const auto key = line.substr(0, separator);
    const auto value = line.substr(separator + 1);

    switch (section) {
      case ParseSection::Engine:
        if (key == "active_preset_slot") {
          if (!ParseSize(value, state.engine.active_preset_slot) ||
              state.engine.active_preset_slot >= state.engine.preset_bank.size()) {
            return false;
          }
        } else if (key == "tempo_bpm") {
          if (!ParseFloat(value, state.engine.tempo_bpm)) {
            return false;
          }
        } else {
          return false;
        }
        break;
      case ParseSection::CurrentPatch:
        if (!PatchSerialization::ApplyKeyValue(state.engine.current_patch, key, value)) {
          return false;
        }
        break;
      case ParseSection::Preset:
        if (!PatchSerialization::ApplyKeyValue(
              state.engine.preset_bank[current_preset_index],
              key,
              value)) {
          return false;
        }
        break;
      case ParseSection::Ui:
        if (!ApplyUiKeyValue(state.ui, key, value)) {
          return false;
        }
        break;
      case ParseSection::None:
        return false;
    }
  }

  return saw_engine && saw_current_patch && saw_ui;
}

}  // namespace fantome
