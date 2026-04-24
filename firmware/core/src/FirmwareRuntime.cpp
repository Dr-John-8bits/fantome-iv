#include "fantome/FirmwareRuntime.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace fantome {

namespace {

constexpr float kPeakDecayPerSecond = 0.80f;
constexpr std::uint8_t kCcMasterVolume = 7;
constexpr std::uint8_t kCcDelayFeedback = 12;
constexpr std::uint8_t kCcDelayMix = 13;
constexpr std::uint8_t kCcResonance = 71;
constexpr std::uint8_t kCcCutoff = 74;
constexpr std::uint8_t kCcReverbMix = 91;
constexpr std::uint8_t kCcChorusDepth = 93;

}  // namespace

RuntimeBootResult FirmwareRuntime::BootWithSession(
  const std::string& session_path,
  float sample_rate)
{
  standalone_ = false;
  ResetCore(sample_rate);

  RuntimeBootResult result;
  result.standalone = false;
  result.session = session_manager_.Boot(session_path, engine_, ui_);
  return result;
}

void FirmwareRuntime::BootStandalone(float sample_rate)
{
  standalone_ = true;
  ResetCore(sample_rate);
  engine_.Reset();
  ui_.Reset(engine_);
}

bool FirmwareRuntime::ApplyHardwareFrame(const HardwareInputFrame& frame)
{
  bool changed = false;

  for (std::size_t index = 0; index < frame.midi_message_count; ++index) {
    HandleMidi(frame.midi_messages[index]);
    changed = true;
  }

  if (controls_.Apply(frame, engine_, ui_)) {
    MarkSessionDirty();
    changed = true;
  }

  changed |= ConsumeUiRuntimeActions();
  return changed;
}

void FirmwareRuntime::HandleMidi(const MidiMessage& message)
{
  engine_.HandleMidi(message);
  midi_activity_hold_s_ = 0.12f;

  if (ShouldMarkDirtyFromMidi(message)) {
    MarkSessionDirty();
  }
}

void FirmwareRuntime::AdvanceDisplay(float delta_seconds)
{
  startup_display_.Advance(delta_seconds);
  ui_.Advance(delta_seconds);
  midi_activity_hold_s_ = std::max(0.0f, midi_activity_hold_s_ - std::max(delta_seconds, 0.0f));
  clip_activity_hold_s_ = std::max(0.0f, clip_activity_hold_s_ - std::max(delta_seconds, 0.0f));
  output_peak_hold_ = std::max(0.0f, output_peak_hold_ - (kPeakDecayPerSecond * std::max(delta_seconds, 0.0f)));
}

void FirmwareRuntime::Render(HardwareAudioBuffer& buffer)
{
  if (!buffer.IsValid()) {
    buffer.Clear();
    return;
  }

  engine_.Render(buffer.left, buffer.right, buffer.frame_count);

  float peak = 0.0f;
  for (std::size_t index = 0; index < buffer.frame_count; ++index) {
    peak = std::max(peak, std::fabs(buffer.left[index]));
    peak = std::max(peak, std::fabs(buffer.right[index]));
  }

  output_peak_hold_ = std::max(output_peak_hold_, peak);
  if (peak >= 0.985f) {
    clip_activity_hold_s_ = 0.18f;
  }
  ++audio_block_count_;
}

void FirmwareRuntime::Render(float* left, float* right, std::size_t frame_count)
{
  HardwareAudioBuffer buffer {left, right, frame_count};
  Render(buffer);
}

HardwareOutputFrame FirmwareRuntime::BuildHardwareOutputFrame() const
{
  HardwareOutputFrame output;
  output.oled = startup_display_.Render(
    oled_,
    ui_,
    engine_,
    standalone_ ? nullptr : &session_manager_.State());
  output.midi_activity = midi_activity_hold_s_ > 0.0f;
  output.preset_dirty = engine_.IsCurrentPresetDirty();
  output.startup_active = startup_display_.ShowingSplash();
  output.output_clip = clip_activity_hold_s_ > 0.0f;
  output.output_peak = output_peak_hold_;
  output.audio_block_count = audio_block_count_;
  output.active_preset_slot = engine_.CurrentPresetSlot();

  if (!standalone_) {
    output.session_dirty = session_manager_.State().checkpoint_dirty;
    output.session_summary = session_manager_.State().last_error.empty()
                               ? session_manager_.State().session_path
                               : session_manager_.State().last_error;
  }

  return output;
}

bool FirmwareRuntime::SaveSessionCheckpoint()
{
  if (standalone_) {
    return false;
  }

  return session_manager_.SaveCheckpoint(engine_, ui_);
}

bool FirmwareRuntime::ReloadSession()
{
  if (standalone_ || session_manager_.State().session_path.empty()) {
    return false;
  }

  const auto boot = session_manager_.Boot(
    session_manager_.State().session_path,
    engine_,
    ui_);
  return boot.mode == SessionBootMode::RestoredFromDisk ||
         boot.mode == SessionBootMode::FreshStart;
}

bool FirmwareRuntime::Shutdown()
{
  if (standalone_) {
    return true;
  }

  return session_manager_.Shutdown(engine_, ui_);
}

FantomeEngine& FirmwareRuntime::Engine()
{
  return engine_;
}

const FantomeEngine& FirmwareRuntime::Engine() const
{
  return engine_;
}

UiState& FirmwareRuntime::Ui()
{
  return ui_;
}

const UiState& FirmwareRuntime::Ui() const
{
  return ui_;
}

const SessionManagerState* FirmwareRuntime::SessionState() const
{
  return standalone_ ? nullptr : &session_manager_.State();
}

const HardwareControlRouter& FirmwareRuntime::Controls() const
{
  return controls_;
}

const StartupDisplayController& FirmwareRuntime::StartupDisplay() const
{
  return startup_display_;
}

void FirmwareRuntime::ResetCore(float sample_rate)
{
  controls_.Reset();
  startup_display_.Reset();
  midi_activity_hold_s_ = 0.0f;
  output_peak_hold_ = 0.0f;
  clip_activity_hold_s_ = 0.0f;
  audio_block_count_ = 0;
  engine_.SetSampleRate(sample_rate);
}

void FirmwareRuntime::MarkSessionDirty()
{
  if (!standalone_) {
    session_manager_.MarkDirty();
  }
}

bool FirmwareRuntime::ShouldMarkDirtyFromMidi(const MidiMessage& message) const
{
  switch (message.type) {
    case MidiMessageType::ControlChange:
      switch (message.data1) {
        case kCcMasterVolume:
        case kCcDelayFeedback:
        case kCcDelayMix:
        case kCcResonance:
        case kCcCutoff:
        case kCcReverbMix:
        case kCcChorusDepth:
          return true;
        default:
          return false;
      }
    case MidiMessageType::ProgramChange:
      return true;
    case MidiMessageType::NoteOn:
    case MidiMessageType::NoteOff:
    case MidiMessageType::PitchBend:
    case MidiMessageType::Clock:
    case MidiMessageType::Start:
    case MidiMessageType::Stop:
    case MidiMessageType::Continue:
    case MidiMessageType::Unknown:
      return false;
  }

  return false;
}

bool FirmwareRuntime::ConsumeUiRuntimeActions()
{
  UiAction action = UiAction::None;
  if (!ui_.ConsumeRuntimeAction(action)) {
    return false;
  }

  switch (action) {
    case UiAction::SaveSession:
      if (SaveSessionCheckpoint()) {
        ui_.SetTransientStatus("Session saved", 1.8f);
        return true;
      }
      ui_.SetTransientStatus("Save failed", 1.8f);
      return false;
    case UiAction::ReloadSession:
      if (ReloadSession()) {
        ui_.SetTransientStatus("Session restored", 1.8f);
        return true;
      }
      ui_.SetTransientStatus("Reload failed", 1.8f);
      return false;
    case UiAction::None:
    case UiAction::LoadPreset:
    case UiAction::SavePreset:
    case UiAction::InitPatch:
      break;
  }

  return false;
}

}  // namespace fantome
