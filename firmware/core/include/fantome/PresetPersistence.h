#pragma once

#include <array>
#include <cstddef>
#include <string>

#include "fantome/Patch.h"

namespace fantome {

class PresetPersistence {
 public:
  static bool SaveBank(
    const std::string& path,
    const std::array<Patch, kPresetCount>& bank,
    std::size_t active_slot);
  static bool LoadBank(
    const std::string& path,
    std::array<Patch, kPresetCount>& bank,
    std::size_t& active_slot);
};

}  // namespace fantome

