#pragma once

#include <ostream>
#include <string>

#include "fantome/Patch.h"

namespace fantome {

class PatchSerialization {
 public:
  static bool ApplyKeyValue(Patch& patch, const std::string& key, const std::string& value);
  static void WriteKeyValues(std::ostream& output, const Patch& patch);
};

}  // namespace fantome
