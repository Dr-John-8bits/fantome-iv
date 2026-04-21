#include "fantome/SessionManager.h"

#include <filesystem>

namespace fantome {

PortableSessionState SessionManager::CaptureState(
  const FantomeEngine& engine,
  const UiState& ui)
{
  return PortableSessionState {
    engine.ExportSessionState(),
    ui.ExportSessionState(),
  };
}

void SessionManager::StartFreshSession(FantomeEngine& engine, UiState& ui)
{
  engine.Reset();
  ui.Reset(engine);
}

SessionBootResult SessionManager::Boot(
  const std::string& session_path,
  FantomeEngine& engine,
  UiState& ui)
{
  state_.active = true;
  state_.session_path = session_path;
  state_.last_shutdown_saved = false;
  state_.checkpoint_dirty = false;
  state_.last_error.clear();

  SessionBootResult result;
  result.session_file_present = std::filesystem::exists(session_path);

  if (!result.session_file_present) {
    StartFreshSession(engine, ui);
    state_.last_boot_mode = SessionBootMode::FreshStart;
    result.mode = SessionBootMode::FreshStart;
    result.message = "fresh session";
    return result;
  }

  PortableSessionState loaded_state;
  if (!SessionPersistence::Load(session_path, loaded_state) ||
      !engine.RestoreSessionState(loaded_state.engine)) {
    StartFreshSession(engine, ui);
    state_.last_boot_mode = SessionBootMode::FallbackFreshStart;
    state_.last_error = "session restore failed";
    result.mode = SessionBootMode::FallbackFreshStart;
    result.message = "fallback to fresh session";
    return result;
  }

  ui.RestoreSessionState(loaded_state.ui, engine);
  state_.last_boot_mode = SessionBootMode::RestoredFromDisk;
  result.mode = SessionBootMode::RestoredFromDisk;
  result.restored = true;
  result.message = "restored session";
  return result;
}

void SessionManager::MarkDirty()
{
  if (!state_.active) {
    return;
  }

  state_.checkpoint_dirty = true;
}

bool SessionManager::SaveCheckpoint(const FantomeEngine& engine, const UiState& ui)
{
  if (!state_.active || state_.session_path.empty()) {
    state_.last_error = "no active session";
    return false;
  }

  if (!SessionPersistence::Save(state_.session_path, CaptureState(engine, ui))) {
    state_.last_error = "checkpoint save failed";
    return false;
  }

  state_.last_error.clear();
  state_.checkpoint_dirty = false;
  return true;
}

bool SessionManager::Shutdown(const FantomeEngine& engine, const UiState& ui)
{
  if (!state_.active || state_.session_path.empty()) {
    state_.last_error = "no active session";
    return false;
  }

  const auto saved = SessionPersistence::Save(state_.session_path, CaptureState(engine, ui));
  state_.last_shutdown_saved = saved;
  if (!saved) {
    state_.last_error = "shutdown save failed";
    return false;
  }

  state_.last_error.clear();
  state_.checkpoint_dirty = false;
  state_.active = false;
  return true;
}

const SessionManagerState& SessionManager::State() const
{
  return state_;
}

}  // namespace fantome
