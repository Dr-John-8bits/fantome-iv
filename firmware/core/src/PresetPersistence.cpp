#include "fantome/PresetPersistence.h"

#include <fstream>
#include <string>

#include "fantome/PatchSerialization.h"

namespace fantome {

namespace {

constexpr const char* kPresetBankHeader = "FANTOME_IV_PRESET_BANK_V1";

bool ParseSize(const std::string& value, std::size_t& output)
{
  try {
    output = static_cast<std::size_t>(std::stoul(value));
    return true;
  } catch (...) {
    return false;
  }
}

void WritePatch(std::ofstream& output, const Patch& patch, std::size_t slot)
{
  output << "[preset " << slot << "]\n";
  PatchSerialization::WriteKeyValues(output, patch);
}

}  // namespace

bool PresetPersistence::SaveBank(
  const std::string& path,
  const std::array<Patch, kPresetCount>& bank,
  std::size_t active_slot)
{
  std::ofstream output(path, std::ios::trunc);
  if (!output.is_open()) {
    return false;
  }

  output << kPresetBankHeader << '\n';
  output << "active_slot=" << active_slot << '\n';
  for (std::size_t slot = 0; slot < bank.size(); ++slot) {
    WritePatch(output, bank[slot], slot);
  }

  return output.good();
}

bool PresetPersistence::LoadBank(
  const std::string& path,
  std::array<Patch, kPresetCount>& bank,
  std::size_t& active_slot)
{
  std::ifstream input(path);
  if (!input.is_open()) {
    return false;
  }

  std::string line;
  if (!std::getline(input, line) || line != kPresetBankHeader) {
    return false;
  }

  bank = {
    MakeInitPatch("Preset 1"),
    MakeInitPatch("Preset 2"),
    MakeInitPatch("Preset 3"),
    MakeInitPatch("Preset 4"),
  };

  std::size_t current_slot = 0;
  bool saw_active_slot = false;

  while (std::getline(input, line)) {
    if (line.empty()) {
      continue;
    }

    if (line.rfind("[preset ", 0) == 0 && line.back() == ']') {
      const auto slot_text = line.substr(8, line.size() - 9);
      if (!ParseSize(slot_text, current_slot) || current_slot >= bank.size()) {
        return false;
      }
      continue;
    }

    const auto separator = line.find('=');
    if (separator == std::string::npos) {
      return false;
    }

    const auto key = line.substr(0, separator);
    const auto value = line.substr(separator + 1);

    if (key == "active_slot") {
      if (!ParseSize(value, active_slot) || active_slot >= bank.size()) {
        return false;
      }
      saw_active_slot = true;
      continue;
    }

    if (!PatchSerialization::ApplyKeyValue(bank[current_slot], key, value)) {
      return false;
    }
  }

  return saw_active_slot;
}

}  // namespace fantome
