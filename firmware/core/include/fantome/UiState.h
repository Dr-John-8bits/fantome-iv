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
