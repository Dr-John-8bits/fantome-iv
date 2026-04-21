#pragma once

#include <string>

#include "fantome/FantomeEngine.h"
#include "fantome/UiState.h"

namespace fantome {

struct PortableSessionState {
  EngineSessionState engine {};
  UiSessionState ui {};
};

class SessionPersistence {
 public:
  static bool Save(const std::string& path, const PortableSessionState& state);
  static bool Load(const std::string& path, PortableSessionState& state);
};

}  // namespace fantome
