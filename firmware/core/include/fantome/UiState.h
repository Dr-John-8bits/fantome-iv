#pragma once

#include <array>
#include <cstddef>
#include <string>

#include "fantome/FantomeEngine.h"

namespace fantome {

enum class UiPage {
  Oscillators,
  Filter,
  AmpEnvelope,
  FilterEnvelope,
  Modulation,
  Effects,
  System,
};

enum class UiInteractionState {
  Navigation,
  Editing,
  Confirmation,
};

enum class ParameterId {
  MasterVolume,
  FilterCutoff,
  FilterResonance,
  AmpAttack,
  AmpDecay,
  AmpSustain,
  AmpRelease,
  OscAWaveform,
  OscAOctave,
  OscAFineTune,
  OscALevel,
  OscBWaveform,
  OscBOctave,
  OscBFineTune,
  OscBLevel,
  OscSync,
  PwmAmount,
  NoiseLevel,
  FilterEnvAmount,
  FilterLfoAmount,
  FilterSampleHoldAmount,
  FilterAttack,
  FilterRelease,
  OscLfoWaveform,
  OscLfoMode,
  OscLfoRateOrDivision,
  OscLfoAmount,
  FilterLfoWaveform,
  FilterLfoMode,
  FilterLfoRateOrDivision,
  ShMode,
  ShRateOrDivision,
  ShAmount,
  ChorusRate,
  ChorusDepth,
  ChorusMix,
  DelayMode,
  DelayTimeOrDivision,
  DelayFeedback,
  DelayMix,
  ReverbMix,
  MidiChannel,
  PlayMode,
  Legato,
  PresetSlot,
};

enum class UiAction {
  None,
  LoadPreset,
  SavePreset,
  InitPatch,
};

struct UiItem {
  bool is_parameter = true;
  ParameterId parameter = ParameterId::MasterVolume;
  UiAction action = UiAction::None;
  const char* label = "";
};

struct PotTakeoverState {
  bool captured = true;
  bool has_physical_value = false;
  float physical_value = 0.0f;
  ParameterId parameter = ParameterId::MasterVolume;
};

struct PotSessionState {
  bool captured = true;
  bool has_physical_value = false;
  float physical_value = 0.0f;
};

struct UiSessionState {
  UiPage current_page = UiPage::Oscillators;
  UiInteractionState interaction_state = UiInteractionState::Navigation;
  std::size_t selected_item_index = 0;
  std::size_t preset_target_slot = 0;
  UiAction pending_action = UiAction::None;
  std::array<PotSessionState, 8> pots {};
};

struct UiDisplayItem {
  std::string label;
  std::string value;
  bool selected = false;
  bool is_action = false;
};

struct UiDisplayModel {
  static constexpr std::size_t kMaxItems = 11;

  std::string preset_name;
  std::size_t active_preset_slot = 0;
  std::size_t target_preset_slot = 0;
  std::string page_label;
  UiInteractionState interaction_state = UiInteractionState::Navigation;
  std::array<UiDisplayItem, kMaxItems> items {};
  std::size_t item_count = 0;
  std::string selected_label;
  std::string selected_value;
  std::string context_pot_label;
  bool context_pot_needs_pickup = false;
  bool any_pot_needs_pickup = false;
  std::string status_text;
};

class UiState {
 public:
  UiState();

  void Reset(const FantomeEngine& engine);
  void NextPage(const FantomeEngine& engine);
  void PreviousPage(const FantomeEngine& engine);
  void RotateEncoder(int delta, FantomeEngine& engine);
  void PressEncoder(FantomeEngine& engine);
  void PressBack(const FantomeEngine& engine);
  void PressAction(FantomeEngine& engine);
  bool MovePot(std::size_t pot_index, float normalized, FantomeEngine& engine);
  void NotifyPresetLoaded(const FantomeEngine& engine);

  UiPage CurrentPage() const;
  UiInteractionState InteractionState() const;
  std::size_t SelectedItemIndex() const;
  std::size_t PresetTargetSlot() const;
  UiAction PendingAction() const;
  const std::array<PotTakeoverState, 8>& Pots() const;
  std::string CurrentPageLabel() const;
  std::string SelectedItemLabel() const;
  UiDisplayModel BuildDisplayModel(const FantomeEngine& engine) const;
  UiSessionState ExportSessionState() const;
  void RestoreSessionState(const UiSessionState& state, const FantomeEngine& engine);

 private:
  static constexpr std::size_t kPotCount = 8;

  const UiItem& CurrentItem() const;
  const UiItem& ContextPotItem() const;
  std::size_t CurrentPageItemCount() const;
  void ClampSelection();
  void AssignFixedPots();
  void AssignContextPot();
  void RearmAllPots(const FantomeEngine& engine);
  void RearmContextPot(const FantomeEngine& engine);
  void ApplyPendingAction(FantomeEngine& engine);
  bool TryCapturePot(
    PotTakeoverState& pot_state,
    float target_value,
    float new_physical_value) const;
  std::string FormatParameterValue(
    ParameterId parameter,
    const FantomeEngine& engine) const;
  std::string FormatItemValue(const UiItem& item, const FantomeEngine& engine) const;
  std::string BuildStatusText(const FantomeEngine& engine) const;
  float ParameterNormalized(ParameterId parameter, const FantomeEngine& engine) const;
  void SetParameterFromNormalized(
    ParameterId parameter,
    float normalized,
    FantomeEngine& engine);
  void StepParameter(
    ParameterId parameter,
    int delta,
    FantomeEngine& engine);

  UiPage current_page_ = UiPage::Oscillators;
  UiInteractionState interaction_state_ = UiInteractionState::Navigation;
  std::size_t selected_item_index_ = 0;
  std::size_t preset_target_slot_ = 0;
  UiAction pending_action_ = UiAction::None;
  std::array<PotTakeoverState, kPotCount> pots_ {};
};

}  // namespace fantome
