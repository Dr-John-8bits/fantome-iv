#pragma once

#include <string>

#include "fantome/FantomeEngine.h"
#include "fantome/SessionPersistence.h"
#include "fantome/UiState.h"

namespace fantome {

enum class SessionBootMode {
  FreshStart,
  RestoredFromDisk,
  FallbackFreshStart,
};

struct SessionBootResult {
  SessionBootMode mode = SessionBootMode::FreshStart;
  bool session_file_present = false;
  bool restored = false;
  std::string message;
};

struct SessionManagerState {
  bool active = false;
  std::string session_path;
  SessionBootMode last_boot_mode = SessionBootMode::FreshStart;
  bool last_shutdown_saved = false;
  std::string last_error;
};

class SessionManager {
 public:
  SessionBootResult Boot(
    const std::string& session_path,
    FantomeEngine& engine,
    UiState& ui);
  bool SaveCheckpoint(const FantomeEngine& engine, const UiState& ui);
  bool Shutdown(const FantomeEngine& engine, const UiState& ui);

  const SessionManagerState& State() const;

 private:
  static PortableSessionState CaptureState(
    const FantomeEngine& engine,
    const UiState& ui);
  void StartFreshSession(FantomeEngine& engine, UiState& ui);

  SessionManagerState state_ {};
};

}  // namespace fantome
