#pragma once

#include <cstddef>
#include <string>

#include "fantome/HardwareIO.h"
#include "fantome/SessionManager.h"
#include "fantome/StartupDisplay.h"

namespace fantome {

struct RuntimeBootResult {
  bool standalone = true;
  SessionBootResult session {};
};

class FirmwareRuntime {
 public:
  RuntimeBootResult BootWithSession(const std::string& session_path, float sample_rate);
  void BootStandalone(float sample_rate);
  bool ApplyHardwareFrame(const HardwareInputFrame& frame);
  void HandleMidi(const MidiMessage& message);
  void AdvanceDisplay(float delta_seconds);
  void Render(float* left, float* right, std::size_t frame_count);
  HardwareOutputFrame BuildHardwareOutputFrame() const;
  bool SaveSessionCheckpoint();
  bool ReloadSession();
  bool Shutdown();

  FantomeEngine& Engine();
  const FantomeEngine& Engine() const;
  UiState& Ui();
  const UiState& Ui() const;
  const SessionManagerState* SessionState() const;
  const HardwareControlRouter& Controls() const;
  const StartupDisplayController& StartupDisplay() const;

 private:
  void ResetCore(float sample_rate);
  void MarkSessionDirty();
  bool ConsumeUiRuntimeActions();
  bool ShouldMarkDirtyFromMidi(const MidiMessage& message) const;

  FantomeEngine engine_ {};
  UiState ui_ {};
  OledTextRenderer oled_ {};
  HardwareControlRouter controls_ {};
  SessionManager session_manager_ {};
  StartupDisplayController startup_display_ {};
  bool standalone_ = true;
  float midi_activity_hold_s_ = 0.0f;
};

}  // namespace fantome
