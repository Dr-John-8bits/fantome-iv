#include "fantome/UiState.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

namespace fantome {

namespace {

using Item = UiItem;

constexpr float kSoftTakeoverThreshold = 0.04f;

constexpr std::array<Item, 11> kOscillatorPageItems {{
  {true, ParameterId::OscAWaveform, UiAction::None, "Osc A Wave"},
  {true, ParameterId::OscAOctave, UiAction::None, "Osc A Oct"},
  {true, ParameterId::OscAFineTune, UiAction::None, "Osc A Fine"},
  {true, ParameterId::OscALevel, UiAction::None, "Osc A Level"},
  {true, ParameterId::OscBWaveform, UiAction::None, "Osc B Wave"},
  {true, ParameterId::OscBOctave, UiAction::None, "Osc B Oct"},
  {true, ParameterId::OscBFineTune, UiAction::None, "Osc B Fine"},
  {true, ParameterId::OscBLevel, UiAction::None, "Osc B Level"},
  {true, ParameterId::OscSync, UiAction::None, "Osc Sync"},
  {true, ParameterId::PwmAmount, UiAction::None, "PWM"},
  {true, ParameterId::NoiseLevel, UiAction::None, "Noise"},
}};

constexpr std::array<Item, 5> kFilterPageItems {{
  {true, ParameterId::FilterCutoff, UiAction::None, "Cutoff"},
  {true, ParameterId::FilterResonance, UiAction::None, "Resonance"},
  {true, ParameterId::FilterEnvAmount, UiAction::None, "Env Amt"},
  {true, ParameterId::FilterLfoAmount, UiAction::None, "LFO Amt"},
  {true, ParameterId::FilterSampleHoldAmount, UiAction::None, "S&H Amt"},
}};

constexpr std::array<Item, 4> kAmpPageItems {{
  {true, ParameterId::AmpAttack, UiAction::None, "Attack"},
  {true, ParameterId::AmpDecay, UiAction::None, "Decay"},
  {true, ParameterId::AmpSustain, UiAction::None, "Sustain"},
  {true, ParameterId::AmpRelease, UiAction::None, "Release"},
}};

constexpr std::array<Item, 3> kFilterEnvPageItems {{
  {true, ParameterId::FilterAttack, UiAction::None, "Flt Attack"},
  {true, ParameterId::FilterRelease, UiAction::None, "Flt Release"},
  {true, ParameterId::FilterEnvAmount, UiAction::None, "Env Amt"},
}};

constexpr std::array<Item, 11> kModulationPageItems {{
  {true, ParameterId::OscLfoWaveform, UiAction::None, "Pitch LFO Wave"},
  {true, ParameterId::OscLfoMode, UiAction::None, "Pitch LFO Mode"},
  {true, ParameterId::OscLfoRateOrDivision, UiAction::None, "Pitch LFO Rate"},
  {true, ParameterId::OscLfoAmount, UiAction::None, "Pitch LFO Amt"},
  {true, ParameterId::FilterLfoWaveform, UiAction::None, "Filter LFO Wave"},
  {true, ParameterId::FilterLfoMode, UiAction::None, "Filter LFO Mode"},
  {true, ParameterId::FilterLfoRateOrDivision, UiAction::None, "Filter LFO Rate"},
  {true, ParameterId::FilterLfoAmount, UiAction::None, "Filter LFO Amt"},
  {true, ParameterId::ShMode, UiAction::None, "S&H Mode"},
  {true, ParameterId::ShRateOrDivision, UiAction::None, "S&H Rate"},
  {true, ParameterId::ShAmount, UiAction::None, "S&H Amt"},
}};

constexpr std::array<Item, 8> kEffectsPageItems {{
  {true, ParameterId::ChorusRate, UiAction::None, "Chorus Rate"},
  {true, ParameterId::ChorusDepth, UiAction::None, "Chorus Depth"},
  {true, ParameterId::ChorusMix, UiAction::None, "Chorus Mix"},
  {true, ParameterId::DelayMode, UiAction::None, "Delay Mode"},
  {true, ParameterId::DelayTimeOrDivision, UiAction::None, "Delay Time"},
  {true, ParameterId::DelayFeedback, UiAction::None, "Delay Feedback"},
  {true, ParameterId::DelayMix, UiAction::None, "Delay Mix"},
  {true, ParameterId::ReverbMix, UiAction::None, "Reverb Mix"},
}};

constexpr std::array<Item, 9> kSystemPageItems {{
  {true, ParameterId::MidiChannel, UiAction::None, "MIDI Ch"},
  {true, ParameterId::PlayMode, UiAction::None, "Play Mode"},
  {true, ParameterId::Legato, UiAction::None, "Legato"},
  {true, ParameterId::PresetSlot, UiAction::None, "User Slot"},
  {false, ParameterId::PresetSlot, UiAction::LoadPreset, "Recall Slot"},
  {false, ParameterId::PresetSlot, UiAction::SavePreset, "Write Slot"},
  {false, ParameterId::PresetSlot, UiAction::SaveSession, "Save Sess"},
  {false, ParameterId::PresetSlot, UiAction::ReloadSession, "Reload Sess"},
  {false, ParameterId::PresetSlot, UiAction::InitPatch, "Init Patch"},
}};

float Clamp01(float value)
{
  return std::clamp(value, 0.0f, 1.0f);
}

float TimeToNormalized(float seconds, float min_seconds, float max_seconds)
{
  const auto clamped = std::clamp(seconds, min_seconds, max_seconds);
  return std::log(clamped / min_seconds) / std::log(max_seconds / min_seconds);
}

float NormalizedToTime(float normalized, float min_seconds, float max_seconds)
{
  return min_seconds * std::pow(max_seconds / min_seconds, Clamp01(normalized));
}

float SignedRangeToNormalized(float value, float min_value, float max_value)
{
  return Clamp01((value - min_value) / (max_value - min_value));
}

float NormalizedToSignedRange(float normalized, float min_value, float max_value)
{
  return min_value + (Clamp01(normalized) * (max_value - min_value));
}

float BoolToNormalized(bool value)
{
  return value ? 1.0f : 0.0f;
}

int RoundedIndex(float normalized, int max_index)
{
  return static_cast<int>(std::round(Clamp01(normalized) * static_cast<float>(max_index)));
}

const char* PageLabel(UiPage page)
{
  switch (page) {
    case UiPage::Oscillators:
      return "Oscillators";
    case UiPage::Filter:
      return "Filter";
    case UiPage::AmpEnvelope:
      return "Amp ADSR";
    case UiPage::FilterEnvelope:
      return "Filter AR";
    case UiPage::Modulation:
      return "LFO + S&H";
    case UiPage::Effects:
      return "Effects";
    case UiPage::System:
      return "System";
  }

  return "Unknown";
}

const Item& ItemForPage(UiPage page, std::size_t index)
{
  switch (page) {
    case UiPage::Oscillators:
      return kOscillatorPageItems[index];
    case UiPage::Filter:
      return kFilterPageItems[index];
    case UiPage::AmpEnvelope:
      return kAmpPageItems[index];
    case UiPage::FilterEnvelope:
      return kFilterEnvPageItems[index];
    case UiPage::Modulation:
      return kModulationPageItems[index];
    case UiPage::Effects:
      return kEffectsPageItems[index];
    case UiPage::System:
      return kSystemPageItems[index];
  }

  return kOscillatorPageItems[0];
}

std::size_t PageItemCount(UiPage page)
{
  switch (page) {
    case UiPage::Oscillators:
      return kOscillatorPageItems.size();
    case UiPage::Filter:
      return kFilterPageItems.size();
    case UiPage::AmpEnvelope:
      return kAmpPageItems.size();
    case UiPage::FilterEnvelope:
      return kFilterEnvPageItems.size();
    case UiPage::Modulation:
      return kModulationPageItems.size();
    case UiPage::Effects:
      return kEffectsPageItems.size();
    case UiPage::System:
      return kSystemPageItems.size();
  }

  return 0;
}

ParameterId DefaultContextParameter(UiPage page)
{
  switch (page) {
    case UiPage::Oscillators:
      return ParameterId::NoiseLevel;
    case UiPage::Filter:
      return ParameterId::FilterEnvAmount;
    case UiPage::AmpEnvelope:
      return ParameterId::AmpAttack;
    case UiPage::FilterEnvelope:
      return ParameterId::FilterEnvAmount;
    case UiPage::Modulation:
      return ParameterId::OscLfoAmount;
    case UiPage::Effects:
      return ParameterId::DelayFeedback;
    case UiPage::System:
      return ParameterId::PresetSlot;
  }

  return ParameterId::MasterVolume;
}

std::string WaveformLabel(Waveform waveform)
{
  switch (waveform) {
    case Waveform::Sine:
      return "Sine";
    case Waveform::Triangle:
      return "Tri";
    case Waveform::Square:
      return "Square";
    case Waveform::Saw:
      return "Saw";
    case Waveform::Noise:
      return "Noise";
  }

  return "Unknown";
}

std::string SyncModeLabel(SyncMode mode)
{
  switch (mode) {
    case SyncMode::Free:
      return "Free";
    case SyncMode::MidiClock:
      return "Clock";
  }

  return "Unknown";
}

std::string DivisionLabel(int division_index)
{
  static constexpr std::array<const char*, 5> kLabels {
    "1/1",
    "1/2",
    "1/4",
    "1/8",
    "1/16",
  };

  return kLabels[static_cast<std::size_t>(std::clamp(division_index, 0, 4))];
}

std::string PlayModeLabel(PlayMode mode)
{
  switch (mode) {
    case PlayMode::Poly:
      return "Poly";
    case PlayMode::Mono:
      return "Mono";
    case PlayMode::Unison:
      return "Unison";
  }

  return "Unknown";
}

std::string BoolLabel(bool enabled)
{
  return enabled ? "On" : "Off";
}

std::string PercentLabel(float normalized)
{
  return std::to_string(static_cast<int>(std::round(Clamp01(normalized) * 100.0f))) + "%";
}

std::string TimeLabel(float seconds)
{
  std::ostringstream stream;
  if (seconds < 1.0f) {
    stream << static_cast<int>(std::round(seconds * 1000.0f)) << "ms";
  } else {
    stream << std::fixed << std::setprecision(seconds < 10.0f ? 2 : 1) << seconds << "s";
  }
  return stream.str();
}

std::string RateLabel(float hz)
{
  std::ostringstream stream;
  stream << std::fixed << std::setprecision(hz < 1.0f ? 2 : 1) << hz << "Hz";
  return stream.str();
}

std::string MillisecondsLabel(float milliseconds)
{
  std::ostringstream stream;
  if (milliseconds < 1000.0f) {
    stream << static_cast<int>(std::round(milliseconds)) << "ms";
  } else {
    stream << std::fixed << std::setprecision(2) << (milliseconds / 1000.0f) << "s";
  }
  return stream.str();
}

std::string OctaveLabel(int octave)
{
  std::ostringstream stream;
  if (octave > 0) {
    stream << '+';
  }
  stream << octave << "oct";
  return stream.str();
}

std::string FineTuneLabel(float cents)
{
  const auto rounded = static_cast<int>(std::round(cents));
  std::ostringstream stream;
  if (rounded > 0) {
    stream << '+';
  }
  stream << rounded << "ct";
  return stream.str();
}

std::string PresetSlotLabel(std::size_t slot)
{
  return "P" + std::to_string(slot + 1);
}

std::string TempoLabel(float bpm)
{
  std::ostringstream stream;
  stream << std::fixed << std::setprecision(0) << bpm << "bpm";
  return stream.str();
}

}  // namespace

UiState::UiState() = default;

void UiState::Reset(const FantomeEngine& engine)
{
  current_page_ = UiPage::Oscillators;
  interaction_state_ = UiInteractionState::Navigation;
  selected_item_index_ = 0;
  preset_target_slot_ = engine.CurrentPresetSlot();
  pending_action_ = UiAction::None;
  emitted_runtime_action_ = UiAction::None;
  AssignFixedPots();
  AssignContextPot();
  for (auto& pot : pots_) {
    pot.captured = true;
  }
}

void UiState::NextPage(const FantomeEngine& engine)
{
  current_page_ = static_cast<UiPage>((static_cast<int>(current_page_) + 1) % 7);
  selected_item_index_ = 0;
  interaction_state_ = UiInteractionState::Navigation;
  pending_action_ = UiAction::None;
  emitted_runtime_action_ = UiAction::None;
  AssignContextPot();
  RearmContextPot(engine);
}

void UiState::PreviousPage(const FantomeEngine& engine)
{
  current_page_ = static_cast<UiPage>((static_cast<int>(current_page_) + 6) % 7);
  selected_item_index_ = 0;
  interaction_state_ = UiInteractionState::Navigation;
  pending_action_ = UiAction::None;
  emitted_runtime_action_ = UiAction::None;
  AssignContextPot();
  RearmContextPot(engine);
}

void UiState::RotateEncoder(int delta, FantomeEngine& engine)
{
  if (delta == 0) {
    return;
  }

  if (interaction_state_ == UiInteractionState::Navigation) {
    const auto item_count = static_cast<int>(CurrentPageItemCount());
    const auto base = static_cast<int>(selected_item_index_);
    const auto wrapped = (base + delta % item_count + item_count) % item_count;
    selected_item_index_ = static_cast<std::size_t>(wrapped);
    AssignContextPot();
    RearmContextPot(engine);
    return;
  }

  if (interaction_state_ == UiInteractionState::Editing && CurrentItem().is_parameter) {
    StepParameter(CurrentItem().parameter, delta, engine);
  }
}

void UiState::PressEncoder(FantomeEngine& engine)
{
  const auto& item = CurrentItem();

  if (interaction_state_ == UiInteractionState::Navigation) {
    if (item.is_parameter) {
      interaction_state_ = UiInteractionState::Editing;
    } else {
      pending_action_ = item.action;
      interaction_state_ = UiInteractionState::Confirmation;
    }
    return;
  }

  if (interaction_state_ == UiInteractionState::Editing) {
    interaction_state_ = UiInteractionState::Navigation;
    return;
  }

  if (interaction_state_ == UiInteractionState::Confirmation) {
    ApplyPendingAction(engine);
    interaction_state_ = UiInteractionState::Navigation;
    pending_action_ = UiAction::None;
  }
}

void UiState::PressBack(const FantomeEngine& engine)
{
  (void)engine;
  interaction_state_ = UiInteractionState::Navigation;
  pending_action_ = UiAction::None;
  emitted_runtime_action_ = UiAction::None;
}

void UiState::PressAction(FantomeEngine& engine)
{
  if (!CurrentItem().is_parameter) {
    PressEncoder(engine);
    return;
  }

  if (current_page_ == UiPage::System) {
    pending_action_ = UiAction::LoadPreset;
    interaction_state_ = UiInteractionState::Confirmation;
  }
}

bool UiState::MovePot(std::size_t pot_index, float normalized, FantomeEngine& engine)
{
  if (pot_index >= pots_.size()) {
    return false;
  }

  auto& pot_state = pots_[pot_index];
  const auto clamped_normalized = Clamp01(normalized);
  const auto target = ParameterNormalized(pot_state.parameter, engine);
  const auto captured = TryCapturePot(pot_state, target, clamped_normalized);
  pot_state.has_physical_value = true;
  pot_state.physical_value = clamped_normalized;

  if (!captured) {
    return false;
  }

  SetParameterFromNormalized(pot_state.parameter, clamped_normalized, engine);
  return true;
}

void UiState::NotifyPresetLoaded(const FantomeEngine& engine)
{
  preset_target_slot_ = engine.CurrentPresetSlot();
  RearmAllPots(engine);
}

void UiState::Advance(float delta_seconds)
{
  transient_status_hold_s_ =
    std::max(0.0f, transient_status_hold_s_ - std::max(delta_seconds, 0.0f));
  if (transient_status_hold_s_ <= 0.0f) {
    transient_status_text_.clear();
  }
}

void UiState::SetTransientStatus(const std::string& text, float hold_seconds)
{
  transient_status_text_ = text;
  transient_status_hold_s_ = std::max(hold_seconds, 0.0f);
}

UiPage UiState::CurrentPage() const
{
  return current_page_;
}

UiInteractionState UiState::InteractionState() const
{
  return interaction_state_;
}

std::size_t UiState::SelectedItemIndex() const
{
  return selected_item_index_;
}

std::size_t UiState::PresetTargetSlot() const
{
  return preset_target_slot_;
}

UiAction UiState::PendingAction() const
{
  return pending_action_;
}

bool UiState::ConsumeRuntimeAction(UiAction& action)
{
  switch (emitted_runtime_action_) {
    case UiAction::SaveSession:
    case UiAction::ReloadSession:
      action = emitted_runtime_action_;
      emitted_runtime_action_ = UiAction::None;
      return true;
    case UiAction::None:
    case UiAction::LoadPreset:
    case UiAction::SavePreset:
    case UiAction::InitPatch:
      break;
  }

  return false;
}

const std::array<PotTakeoverState, 8>& UiState::Pots() const
{
  return pots_;
}

std::string UiState::CurrentPageLabel() const
{
  return PageLabel(current_page_);
}

std::string UiState::SelectedItemLabel() const
{
  return CurrentItem().label;
}

UiDisplayModel UiState::BuildDisplayModel(const FantomeEngine& engine) const
{
  UiDisplayModel model;
  model.preset_name = engine.CurrentPatch().name;
  model.active_preset_slot = engine.CurrentPresetSlot();
  model.target_preset_slot = preset_target_slot_;
  model.page_label = CurrentPageLabel();
  model.interaction_state = interaction_state_;
  model.current_patch_dirty = engine.IsCurrentPresetDirty();
  model.item_count = CurrentPageItemCount();
  model.selected_label = CurrentItem().label;
  model.selected_value = FormatItemValue(CurrentItem(), engine);
  model.context_pot_label = ContextPotItem().label;
  model.context_pot_needs_pickup = !pots_[7].captured;
  model.any_pot_needs_pickup = std::any_of(
    pots_.begin(),
    pots_.end(),
    [](const PotTakeoverState& pot) {
      return !pot.captured;
    });
  model.status_text = BuildStatusText(engine);

  for (std::size_t index = 0; index < model.item_count; ++index) {
    const auto& item = ItemForPage(current_page_, index);
    auto& display_item = model.items[index];
    display_item.label = item.label;
    display_item.value = FormatItemValue(item, engine);
    display_item.selected = index == selected_item_index_;
    display_item.is_action = !item.is_parameter;
  }

  return model;
}

UiSessionState UiState::ExportSessionState() const
{
  UiSessionState state;
  state.current_page = current_page_;
  state.interaction_state = interaction_state_;
  state.selected_item_index = selected_item_index_;
  state.preset_target_slot = preset_target_slot_;
  state.pending_action = pending_action_;

  for (std::size_t index = 0; index < pots_.size(); ++index) {
    state.pots[index].captured = pots_[index].captured;
    state.pots[index].has_physical_value = pots_[index].has_physical_value;
    state.pots[index].physical_value = pots_[index].physical_value;
  }

  return state;
}

void UiState::RestoreSessionState(const UiSessionState& state, const FantomeEngine& engine)
{
  current_page_ = state.current_page;
  interaction_state_ = state.interaction_state;
  selected_item_index_ = state.selected_item_index;
  preset_target_slot_ = std::min(state.preset_target_slot, kPresetCount - 1);
  pending_action_ = state.pending_action;
  emitted_runtime_action_ = UiAction::None;
  transient_status_text_.clear();
  transient_status_hold_s_ = 0.0f;

  ClampSelection();
  AssignFixedPots();
  AssignContextPot();

  for (std::size_t index = 0; index < pots_.size(); ++index) {
    pots_[index].captured = state.pots[index].captured;
    pots_[index].has_physical_value = state.pots[index].has_physical_value;
    pots_[index].physical_value = Clamp01(state.pots[index].physical_value);
  }

  const auto& item = CurrentItem();
  if (interaction_state_ == UiInteractionState::Editing && !item.is_parameter) {
    interaction_state_ = UiInteractionState::Navigation;
  }

  if (interaction_state_ == UiInteractionState::Confirmation) {
    if (item.is_parameter) {
      interaction_state_ = UiInteractionState::Navigation;
      pending_action_ = UiAction::None;
    } else {
      pending_action_ = item.action;
    }
  } else if (item.is_parameter) {
    pending_action_ = UiAction::None;
  }

  if (preset_target_slot_ >= engine.PresetBank().size()) {
    preset_target_slot_ = engine.CurrentPresetSlot();
  }
}

const UiItem& UiState::CurrentItem() const
{
  return ItemForPage(current_page_, selected_item_index_);
}

const UiItem& UiState::ContextPotItem() const
{
  if (CurrentItem().is_parameter) {
    return CurrentItem();
  }

  const auto parameter = DefaultContextParameter(current_page_);
  switch (parameter) {
    case ParameterId::PresetSlot:
      return kSystemPageItems[3];
    case ParameterId::DelayFeedback:
      return kEffectsPageItems[5];
    case ParameterId::OscLfoAmount:
      return kModulationPageItems[3];
    case ParameterId::FilterEnvAmount:
      return kFilterPageItems[2];
    case ParameterId::AmpAttack:
      return kAmpPageItems[0];
    case ParameterId::NoiseLevel:
      return kOscillatorPageItems[10];
    default:
      return CurrentItem();
  }
}

std::size_t UiState::CurrentPageItemCount() const
{
  return PageItemCount(current_page_);
}

void UiState::ClampSelection()
{
  if (selected_item_index_ >= CurrentPageItemCount()) {
    selected_item_index_ = 0;
  }
}

void UiState::AssignFixedPots()
{
  pots_[0].parameter = ParameterId::MasterVolume;
  pots_[1].parameter = ParameterId::FilterCutoff;
  pots_[2].parameter = ParameterId::FilterResonance;
  pots_[3].parameter = ParameterId::AmpAttack;
  pots_[4].parameter = ParameterId::AmpDecay;
  pots_[5].parameter = ParameterId::AmpSustain;
  pots_[6].parameter = ParameterId::AmpRelease;
}

void UiState::AssignContextPot()
{
  ClampSelection();
  pots_[7].parameter = ContextPotItem().parameter;
}

void UiState::RearmAllPots(const FantomeEngine& engine)
{
  (void)engine;
  AssignFixedPots();
  AssignContextPot();
  for (auto& pot : pots_) {
    pot.captured = false;
  }
}

void UiState::RearmContextPot(const FantomeEngine& engine)
{
  (void)engine;
  pots_[7].captured = false;
}

void UiState::ApplyPendingAction(FantomeEngine& engine)
{
  switch (pending_action_) {
    case UiAction::LoadPreset:
      if (engine.LoadPreset(preset_target_slot_)) {
        NotifyPresetLoaded(engine);
        SetTransientStatus("Loaded " + PresetSlotLabel(preset_target_slot_), 1.4f);
      }
      break;
    case UiAction::SavePreset:
      engine.SavePreset(preset_target_slot_);
      preset_target_slot_ = engine.CurrentPresetSlot();
      SetTransientStatus("Saved " + PresetSlotLabel(preset_target_slot_), 1.4f);
      break;
    case UiAction::SaveSession:
    case UiAction::ReloadSession:
      emitted_runtime_action_ = pending_action_;
      break;
    case UiAction::InitPatch:
      engine.InitializeCurrentPatch();
      RearmAllPots(engine);
      SetTransientStatus("Patch reset", 1.4f);
      break;
    case UiAction::None:
      break;
  }
}

bool UiState::TryCapturePot(
  PotTakeoverState& pot_state,
  float target_value,
  float new_physical_value) const
{
  if (pot_state.captured) {
    return true;
  }

  if (!pot_state.has_physical_value) {
    return std::fabs(new_physical_value - target_value) <= kSoftTakeoverThreshold;
  }

  const auto previous = pot_state.physical_value;
  const auto crossed_target =
    (previous <= target_value && new_physical_value >= target_value) ||
    (previous >= target_value && new_physical_value <= target_value);

  return crossed_target || std::fabs(new_physical_value - target_value) <= kSoftTakeoverThreshold;
}

std::string UiState::FormatParameterValue(
  ParameterId parameter,
  const FantomeEngine& engine) const
{
  const auto& patch = engine.CurrentPatch();

  switch (parameter) {
    case ParameterId::MasterVolume:
      return PercentLabel(patch.master_volume);
    case ParameterId::FilterCutoff:
      return PercentLabel(patch.filter.cutoff);
    case ParameterId::FilterResonance:
      return PercentLabel(patch.filter.resonance);
    case ParameterId::AmpAttack:
      return TimeLabel(patch.amp_env.attack_s);
    case ParameterId::AmpDecay:
      return TimeLabel(patch.amp_env.decay_s);
    case ParameterId::AmpSustain:
      return PercentLabel(patch.amp_env.sustain);
    case ParameterId::AmpRelease:
      return TimeLabel(patch.amp_env.release_s);
    case ParameterId::OscAWaveform:
      return WaveformLabel(patch.osc_a.waveform);
    case ParameterId::OscAOctave:
      return OctaveLabel(patch.osc_a.octave);
    case ParameterId::OscAFineTune:
      return FineTuneLabel(patch.osc_a.fine_tune_cents);
    case ParameterId::OscALevel:
      return PercentLabel(patch.osc_a.level);
    case ParameterId::OscBWaveform:
      return WaveformLabel(patch.osc_b.waveform);
    case ParameterId::OscBOctave:
      return OctaveLabel(patch.osc_b.octave);
    case ParameterId::OscBFineTune:
      return FineTuneLabel(patch.osc_b.fine_tune_cents);
    case ParameterId::OscBLevel:
      return PercentLabel(patch.osc_b.level);
    case ParameterId::OscSync:
      return BoolLabel(patch.osc_a.sync_enabled || patch.osc_b.sync_enabled);
    case ParameterId::PwmAmount:
      return PercentLabel(0.5f * (patch.osc_a.pwm + patch.osc_b.pwm));
    case ParameterId::NoiseLevel:
      return PercentLabel(patch.noise_level);
    case ParameterId::FilterEnvAmount:
      return PercentLabel(patch.filter.env_amount);
    case ParameterId::FilterLfoAmount:
      return PercentLabel(std::max(patch.filter.lfo_amount, patch.filter_lfo.amount));
    case ParameterId::FilterSampleHoldAmount:
      return PercentLabel(
        std::max(patch.filter.sample_hold_amount, patch.filter_sample_hold.amount));
    case ParameterId::FilterAttack:
      return TimeLabel(patch.filter_env.attack_s);
    case ParameterId::FilterRelease:
      return TimeLabel(patch.filter_env.release_s);
    case ParameterId::OscLfoWaveform:
      return WaveformLabel(patch.osc_lfo.waveform);
    case ParameterId::OscLfoMode:
      return SyncModeLabel(patch.osc_lfo.sync_mode);
    case ParameterId::OscLfoRateOrDivision:
      return patch.osc_lfo.sync_mode == SyncMode::MidiClock
               ? DivisionLabel(patch.osc_lfo.division_index)
               : RateLabel(patch.osc_lfo.rate_hz);
    case ParameterId::OscLfoAmount:
      return PercentLabel(patch.osc_lfo.amount);
    case ParameterId::FilterLfoWaveform:
      return WaveformLabel(patch.filter_lfo.waveform);
    case ParameterId::FilterLfoMode:
      return SyncModeLabel(patch.filter_lfo.sync_mode);
    case ParameterId::FilterLfoRateOrDivision:
      return patch.filter_lfo.sync_mode == SyncMode::MidiClock
               ? DivisionLabel(patch.filter_lfo.division_index)
               : RateLabel(patch.filter_lfo.rate_hz);
    case ParameterId::ShMode:
      return SyncModeLabel(patch.filter_sample_hold.sync_mode);
    case ParameterId::ShRateOrDivision:
      return patch.filter_sample_hold.sync_mode == SyncMode::MidiClock
               ? DivisionLabel(patch.filter_sample_hold.division_index)
               : RateLabel(patch.filter_sample_hold.rate_hz);
    case ParameterId::ShAmount:
      return PercentLabel(
        std::max(patch.filter_sample_hold.amount, patch.filter.sample_hold_amount));
    case ParameterId::ChorusRate:
      return RateLabel(patch.chorus.rate_hz);
    case ParameterId::ChorusDepth:
      return PercentLabel(patch.chorus.depth);
    case ParameterId::ChorusMix:
      return PercentLabel(patch.chorus.mix);
    case ParameterId::DelayMode:
      return SyncModeLabel(patch.delay.sync_mode);
    case ParameterId::DelayTimeOrDivision:
      return patch.delay.sync_mode == SyncMode::MidiClock
               ? DivisionLabel(patch.delay.division_index)
               : MillisecondsLabel(patch.delay.time_ms);
    case ParameterId::DelayFeedback:
      return PercentLabel(patch.delay.feedback);
    case ParameterId::DelayMix:
      return PercentLabel(patch.delay.mix);
    case ParameterId::ReverbMix:
      return PercentLabel(patch.reverb.mix);
    case ParameterId::MidiChannel:
      return "Ch " + std::to_string(patch.midi_channel);
    case ParameterId::PlayMode:
      return PlayModeLabel(patch.play_mode);
    case ParameterId::Legato:
      return BoolLabel(patch.legato_enabled);
    case ParameterId::PresetSlot:
      return PresetSlotLabel(preset_target_slot_);
  }

  return "";
}

std::string UiState::FormatItemValue(const UiItem& item, const FantomeEngine& engine) const
{
  if (item.is_parameter) {
    return FormatParameterValue(item.parameter, engine);
  }

  switch (item.action) {
    case UiAction::LoadPreset:
    case UiAction::SavePreset:
      return PresetSlotLabel(preset_target_slot_) + " " +
             engine.PresetBank()[preset_target_slot_].name;
    case UiAction::SaveSession:
      return "Checkpoint";
    case UiAction::ReloadSession:
      return "Restore";
    case UiAction::InitPatch:
      return "Reset";
    case UiAction::None:
      break;
  }

  return "";
}

std::string UiState::BuildStatusText(const FantomeEngine& engine) const
{
  if (transient_status_hold_s_ > 0.0f && !transient_status_text_.empty()) {
    return transient_status_text_;
  }

  const auto& patch = engine.CurrentPatch();

  switch (current_page_) {
    case UiPage::System:
      return "Slot " + PresetSlotLabel(engine.CurrentPresetSlot()) +
             (engine.IsCurrentPresetDirty() ? " unsaved" : " saved");
    case UiPage::Modulation:
      if (patch.osc_lfo.sync_mode == SyncMode::MidiClock ||
          patch.filter_lfo.sync_mode == SyncMode::MidiClock ||
          patch.filter_sample_hold.sync_mode == SyncMode::MidiClock) {
        return engine.Transport().running
                 ? "Clock " + TempoLabel(engine.Transport().tempo_bpm)
                 : "Clock idle";
      }
      break;
    case UiPage::Effects:
      if (patch.delay.sync_mode == SyncMode::MidiClock) {
        return engine.Transport().running
                 ? "Delay " + DivisionLabel(patch.delay.division_index) + " " +
                     TempoLabel(engine.Transport().tempo_bpm)
                 : "Delay clock idle";
      }
      break;
    case UiPage::Oscillators:
    case UiPage::Filter:
    case UiPage::AmpEnvelope:
    case UiPage::FilterEnvelope:
      break;
  }

  return "K8 " + std::string(ContextPotItem().label);
}

float UiState::ParameterNormalized(ParameterId parameter, const FantomeEngine& engine) const
{
  const auto& patch = engine.CurrentPatch();
  switch (parameter) {
    case ParameterId::MasterVolume:
      return Clamp01(patch.master_volume);
    case ParameterId::FilterCutoff:
      return Clamp01(patch.filter.cutoff);
    case ParameterId::FilterResonance:
      return Clamp01(patch.filter.resonance);
    case ParameterId::AmpAttack:
      return TimeToNormalized(patch.amp_env.attack_s, 0.001f, 5.0f);
    case ParameterId::AmpDecay:
      return TimeToNormalized(patch.amp_env.decay_s, 0.001f, 5.0f);
    case ParameterId::AmpSustain:
      return Clamp01(patch.amp_env.sustain);
    case ParameterId::AmpRelease:
      return TimeToNormalized(patch.amp_env.release_s, 0.001f, 5.0f);
    case ParameterId::OscAWaveform:
      return static_cast<float>(static_cast<int>(patch.osc_a.waveform)) / 4.0f;
    case ParameterId::OscAOctave:
      return SignedRangeToNormalized(static_cast<float>(patch.osc_a.octave), -2.0f, 2.0f);
    case ParameterId::OscAFineTune:
      return SignedRangeToNormalized(patch.osc_a.fine_tune_cents, -50.0f, 50.0f);
    case ParameterId::OscALevel:
      return Clamp01(patch.osc_a.level);
    case ParameterId::OscBWaveform:
      return static_cast<float>(static_cast<int>(patch.osc_b.waveform)) / 4.0f;
    case ParameterId::OscBOctave:
      return SignedRangeToNormalized(static_cast<float>(patch.osc_b.octave), -2.0f, 2.0f);
    case ParameterId::OscBFineTune:
      return SignedRangeToNormalized(patch.osc_b.fine_tune_cents, -50.0f, 50.0f);
    case ParameterId::OscBLevel:
      return Clamp01(patch.osc_b.level);
    case ParameterId::OscSync:
      return BoolToNormalized(patch.osc_a.sync_enabled || patch.osc_b.sync_enabled);
    case ParameterId::PwmAmount:
      return Clamp01(0.5f * (patch.osc_a.pwm + patch.osc_b.pwm));
    case ParameterId::NoiseLevel:
      return Clamp01(patch.noise_level);
    case ParameterId::FilterEnvAmount:
      return Clamp01(patch.filter.env_amount);
    case ParameterId::FilterLfoAmount:
      return Clamp01(std::max(patch.filter.lfo_amount, patch.filter_lfo.amount));
    case ParameterId::FilterSampleHoldAmount:
      return Clamp01(std::max(patch.filter.sample_hold_amount, patch.filter_sample_hold.amount));
    case ParameterId::FilterAttack:
      return TimeToNormalized(patch.filter_env.attack_s, 0.001f, 5.0f);
    case ParameterId::FilterRelease:
      return TimeToNormalized(patch.filter_env.release_s, 0.001f, 5.0f);
    case ParameterId::OscLfoWaveform:
      return static_cast<float>(static_cast<int>(patch.osc_lfo.waveform)) / 4.0f;
    case ParameterId::OscLfoMode:
      return static_cast<float>(static_cast<int>(patch.osc_lfo.sync_mode));
    case ParameterId::OscLfoRateOrDivision:
      if (patch.osc_lfo.sync_mode == SyncMode::MidiClock) {
        return static_cast<float>(patch.osc_lfo.division_index) / 4.0f;
      }
      return TimeToNormalized(patch.osc_lfo.rate_hz, 0.05f, 20.0f);
    case ParameterId::OscLfoAmount:
      return Clamp01(patch.osc_lfo.amount);
    case ParameterId::FilterLfoWaveform:
      return static_cast<float>(static_cast<int>(patch.filter_lfo.waveform)) / 4.0f;
    case ParameterId::FilterLfoMode:
      return static_cast<float>(static_cast<int>(patch.filter_lfo.sync_mode));
    case ParameterId::FilterLfoRateOrDivision:
      if (patch.filter_lfo.sync_mode == SyncMode::MidiClock) {
        return static_cast<float>(patch.filter_lfo.division_index) / 4.0f;
      }
      return TimeToNormalized(patch.filter_lfo.rate_hz, 0.05f, 20.0f);
    case ParameterId::ShMode:
      return static_cast<float>(static_cast<int>(patch.filter_sample_hold.sync_mode));
    case ParameterId::ShRateOrDivision:
      if (patch.filter_sample_hold.sync_mode == SyncMode::MidiClock) {
        return static_cast<float>(patch.filter_sample_hold.division_index) / 4.0f;
      }
      return TimeToNormalized(patch.filter_sample_hold.rate_hz, 0.05f, 20.0f);
    case ParameterId::ShAmount:
      return Clamp01(std::max(patch.filter_sample_hold.amount, patch.filter.sample_hold_amount));
    case ParameterId::ChorusRate:
      return TimeToNormalized(patch.chorus.rate_hz, 0.05f, 8.0f);
    case ParameterId::ChorusDepth:
      return Clamp01(patch.chorus.depth);
    case ParameterId::ChorusMix:
      return Clamp01(patch.chorus.mix);
    case ParameterId::DelayMode:
      return static_cast<float>(static_cast<int>(patch.delay.sync_mode));
    case ParameterId::DelayTimeOrDivision:
      if (patch.delay.sync_mode == SyncMode::MidiClock) {
        return static_cast<float>(patch.delay.division_index) / 4.0f;
      }
      return TimeToNormalized(patch.delay.time_ms, 10.0f, 2000.0f);
    case ParameterId::DelayFeedback:
      return Clamp01(patch.delay.feedback);
    case ParameterId::DelayMix:
      return Clamp01(patch.delay.mix);
    case ParameterId::ReverbMix:
      return Clamp01(patch.reverb.mix);
    case ParameterId::MidiChannel:
      return static_cast<float>(patch.midi_channel - 1) / 15.0f;
    case ParameterId::PlayMode:
      return static_cast<float>(static_cast<int>(patch.play_mode)) / 2.0f;
    case ParameterId::Legato:
      return BoolToNormalized(patch.legato_enabled);
    case ParameterId::PresetSlot:
      return static_cast<float>(preset_target_slot_) / 3.0f;
  }

  return 0.0f;
}

void UiState::SetParameterFromNormalized(
  ParameterId parameter,
  float normalized,
  FantomeEngine& engine)
{
  auto& patch = engine.CurrentPatchMutable();
  const auto n = Clamp01(normalized);

  switch (parameter) {
    case ParameterId::MasterVolume:
      patch.master_volume = n;
      break;
    case ParameterId::FilterCutoff:
      patch.filter.cutoff = n;
      break;
    case ParameterId::FilterResonance:
      patch.filter.resonance = n;
      break;
    case ParameterId::AmpAttack:
      patch.amp_env.attack_s = NormalizedToTime(n, 0.001f, 5.0f);
      break;
    case ParameterId::AmpDecay:
      patch.amp_env.decay_s = NormalizedToTime(n, 0.001f, 5.0f);
      break;
    case ParameterId::AmpSustain:
      patch.amp_env.sustain = n;
      break;
    case ParameterId::AmpRelease:
      patch.amp_env.release_s = NormalizedToTime(n, 0.001f, 5.0f);
      break;
    case ParameterId::OscAWaveform:
      patch.osc_a.waveform = static_cast<Waveform>(RoundedIndex(n, 4));
      break;
    case ParameterId::OscAOctave:
      patch.osc_a.octave = RoundedIndex(n, 4) - 2;
      break;
    case ParameterId::OscAFineTune:
      patch.osc_a.fine_tune_cents = NormalizedToSignedRange(n, -50.0f, 50.0f);
      break;
    case ParameterId::OscALevel:
      patch.osc_a.level = n;
      break;
    case ParameterId::OscBWaveform:
      patch.osc_b.waveform = static_cast<Waveform>(RoundedIndex(n, 4));
      break;
    case ParameterId::OscBOctave:
      patch.osc_b.octave = RoundedIndex(n, 4) - 2;
      break;
    case ParameterId::OscBFineTune:
      patch.osc_b.fine_tune_cents = NormalizedToSignedRange(n, -50.0f, 50.0f);
      break;
    case ParameterId::OscBLevel:
      patch.osc_b.level = n;
      break;
    case ParameterId::OscSync: {
      const auto enabled = n >= 0.5f;
      patch.osc_a.sync_enabled = enabled;
      patch.osc_b.sync_enabled = enabled;
      break;
    }
    case ParameterId::PwmAmount:
      patch.osc_a.pwm = n;
      patch.osc_b.pwm = n;
      break;
    case ParameterId::NoiseLevel:
      patch.noise_level = n;
      break;
    case ParameterId::FilterEnvAmount:
      patch.filter.env_amount = n;
      break;
    case ParameterId::FilterLfoAmount:
      patch.filter.lfo_amount = n;
      patch.filter_lfo.amount = n;
      break;
    case ParameterId::FilterSampleHoldAmount:
      patch.filter.sample_hold_amount = n;
      patch.filter_sample_hold.amount = n;
      break;
    case ParameterId::FilterAttack:
      patch.filter_env.attack_s = NormalizedToTime(n, 0.001f, 5.0f);
      break;
    case ParameterId::FilterRelease:
      patch.filter_env.release_s = NormalizedToTime(n, 0.001f, 5.0f);
      break;
    case ParameterId::OscLfoWaveform:
      patch.osc_lfo.waveform = static_cast<Waveform>(RoundedIndex(n, 4));
      break;
    case ParameterId::OscLfoMode:
      patch.osc_lfo.sync_mode = static_cast<SyncMode>(RoundedIndex(n, 1));
      break;
    case ParameterId::OscLfoRateOrDivision:
      if (patch.osc_lfo.sync_mode == SyncMode::MidiClock) {
        patch.osc_lfo.division_index = RoundedIndex(n, 4);
      } else {
        patch.osc_lfo.rate_hz = NormalizedToTime(n, 0.05f, 20.0f);
      }
      break;
    case ParameterId::OscLfoAmount:
      patch.osc_lfo.amount = n;
      break;
    case ParameterId::FilterLfoWaveform:
      patch.filter_lfo.waveform = static_cast<Waveform>(RoundedIndex(n, 4));
      break;
    case ParameterId::FilterLfoMode:
      patch.filter_lfo.sync_mode = static_cast<SyncMode>(RoundedIndex(n, 1));
      break;
    case ParameterId::FilterLfoRateOrDivision:
      if (patch.filter_lfo.sync_mode == SyncMode::MidiClock) {
        patch.filter_lfo.division_index = RoundedIndex(n, 4);
      } else {
        patch.filter_lfo.rate_hz = NormalizedToTime(n, 0.05f, 20.0f);
      }
      break;
    case ParameterId::ShMode:
      patch.filter_sample_hold.sync_mode = static_cast<SyncMode>(RoundedIndex(n, 1));
      break;
    case ParameterId::ShRateOrDivision:
      if (patch.filter_sample_hold.sync_mode == SyncMode::MidiClock) {
        patch.filter_sample_hold.division_index = RoundedIndex(n, 4);
      } else {
        patch.filter_sample_hold.rate_hz = NormalizedToTime(n, 0.05f, 20.0f);
      }
      break;
    case ParameterId::ShAmount:
      patch.filter_sample_hold.amount = n;
      patch.filter.sample_hold_amount = n;
      break;
    case ParameterId::ChorusRate:
      patch.chorus.rate_hz = NormalizedToTime(n, 0.05f, 8.0f);
      break;
    case ParameterId::ChorusDepth:
      patch.chorus.depth = n;
      break;
    case ParameterId::ChorusMix:
      patch.chorus.mix = n;
      break;
    case ParameterId::DelayMode:
      patch.delay.sync_mode = static_cast<SyncMode>(RoundedIndex(n, 1));
      break;
    case ParameterId::DelayTimeOrDivision:
      if (patch.delay.sync_mode == SyncMode::MidiClock) {
        patch.delay.division_index = RoundedIndex(n, 4);
      } else {
        patch.delay.time_ms = NormalizedToTime(n, 10.0f, 2000.0f);
      }
      break;
    case ParameterId::DelayFeedback:
      patch.delay.feedback = n;
      break;
    case ParameterId::DelayMix:
      patch.delay.mix = n;
      break;
    case ParameterId::ReverbMix:
      patch.reverb.mix = n;
      break;
    case ParameterId::MidiChannel:
      patch.midi_channel = static_cast<std::uint8_t>(RoundedIndex(n, 15) + 1);
      break;
    case ParameterId::PlayMode:
      patch.play_mode = static_cast<PlayMode>(RoundedIndex(n, 2));
      break;
    case ParameterId::Legato:
      patch.legato_enabled = n >= 0.5f;
      break;
    case ParameterId::PresetSlot:
      preset_target_slot_ = static_cast<std::size_t>(RoundedIndex(n, 3));
      break;
  }
}

void UiState::StepParameter(
  ParameterId parameter,
  int delta,
  FantomeEngine& engine)
{
  const auto current = ParameterNormalized(parameter, engine);
  float step = 0.02f;

  switch (parameter) {
    case ParameterId::OscAWaveform:
    case ParameterId::OscBWaveform:
    case ParameterId::OscLfoWaveform:
    case ParameterId::FilterLfoWaveform:
      step = 0.25f;
      break;
    case ParameterId::OscAOctave:
    case ParameterId::OscBOctave:
      step = 0.25f;
      break;
    case ParameterId::OscSync:
    case ParameterId::OscLfoMode:
    case ParameterId::FilterLfoMode:
    case ParameterId::ShMode:
    case ParameterId::DelayMode:
    case ParameterId::Legato:
      step = 1.0f;
      break;
    case ParameterId::OscLfoRateOrDivision:
      step = engine.CurrentPatch().osc_lfo.sync_mode == SyncMode::MidiClock ? 0.25f : 0.02f;
      break;
    case ParameterId::FilterLfoRateOrDivision:
      step = engine.CurrentPatch().filter_lfo.sync_mode == SyncMode::MidiClock ? 0.25f : 0.02f;
      break;
    case ParameterId::ShRateOrDivision:
      step = engine.CurrentPatch().filter_sample_hold.sync_mode == SyncMode::MidiClock ? 0.25f : 0.02f;
      break;
    case ParameterId::DelayTimeOrDivision:
      step = engine.CurrentPatch().delay.sync_mode == SyncMode::MidiClock ? 0.25f : 0.02f;
      break;
    case ParameterId::MidiChannel:
      step = 1.0f / 15.0f;
      break;
    case ParameterId::PlayMode:
      step = 0.5f;
      break;
    case ParameterId::PresetSlot:
      step = 1.0f / 3.0f;
      break;
    default:
      break;
  }

  SetParameterFromNormalized(parameter, current + (static_cast<float>(delta) * step), engine);
}

}  // namespace fantome
