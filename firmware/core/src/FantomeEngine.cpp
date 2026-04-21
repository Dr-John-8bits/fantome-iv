#include "fantome/FantomeEngine.h"

#include <algorithm>
#include <cstdint>

namespace fantome {

namespace {

constexpr std::uint8_t kCcModWheel = 1;
constexpr std::uint8_t kCcMasterVolume = 7;
constexpr std::uint8_t kCcDelayFeedback = 12;
constexpr std::uint8_t kCcDelayMix = 13;
constexpr std::uint8_t kCcSustain = 64;
constexpr std::uint8_t kCcResonance = 71;
constexpr std::uint8_t kCcCutoff = 74;
constexpr std::uint8_t kCcReverbMix = 91;
constexpr std::uint8_t kCcChorusDepth = 93;
constexpr std::uint8_t kCcAllNotesOff = 123;

}  // namespace

FantomeEngine::FantomeEngine()
{
  Reset();
}

void FantomeEngine::Reset()
{
  preset_bank_ = MakeFactoryPresetBank();
  patch_ = preset_bank_[0];
  allocator_.AllNotesOff();
  performance_ = PerformanceState {};
  transport_ = TransportState {};
}

void FantomeEngine::HandleMidi(const MidiMessage& message)
{
  switch (message.type) {
    case MidiMessageType::Clock:
      ++transport_.midi_clock_pulses;
      break;
    case MidiMessageType::Start:
      transport_.running = true;
      transport_.midi_clock_pulses = 0;
      break;
    case MidiMessageType::Stop:
      transport_.running = false;
      break;
    case MidiMessageType::Continue:
      transport_.running = true;
      break;
    default:
      if (!MatchesCurrentChannel(message)) {
        return;
      }
      break;
  }

  switch (message.type) {
    case MidiMessageType::NoteOn:
      HandleNoteOn(message.data1, message.data2);
      break;
    case MidiMessageType::NoteOff:
      HandleNoteOff(message.data1);
      break;
    case MidiMessageType::ControlChange:
      HandleControlChange(message.data1, message.data2);
      break;
    case MidiMessageType::ProgramChange:
      HandleProgramChange(message.data1);
      break;
    case MidiMessageType::PitchBend:
      HandlePitchBend(message.value);
      break;
    case MidiMessageType::Clock:
    case MidiMessageType::Start:
    case MidiMessageType::Stop:
    case MidiMessageType::Continue:
    case MidiMessageType::Unknown:
      break;
  }
}

void FantomeEngine::SetClockTempoBpm(float bpm)
{
  transport_.tempo_bpm = std::clamp(bpm, 30.0f, 300.0f);
}

void FantomeEngine::SavePreset(std::size_t slot)
{
  if (slot >= preset_bank_.size()) {
    return;
  }
  preset_bank_[slot] = patch_;
}

bool FantomeEngine::LoadPreset(std::size_t slot)
{
  if (slot >= preset_bank_.size()) {
    return false;
  }

  patch_ = preset_bank_[slot];
  allocator_.AllNotesOff();
  performance_.sustain = false;
  return true;
}

Patch& FantomeEngine::CurrentPatchMutable()
{
  return patch_;
}

const Patch& FantomeEngine::CurrentPatch() const
{
  return patch_;
}

const std::array<Patch, kPresetCount>& FantomeEngine::PresetBank() const
{
  return preset_bank_;
}

const std::array<VoiceState, kVoiceCount>& FantomeEngine::Voices() const
{
  return allocator_.Voices();
}

const TransportState& FantomeEngine::Transport() const
{
  return transport_;
}

const PerformanceState& FantomeEngine::Performance() const
{
  return performance_;
}

bool FantomeEngine::MatchesCurrentChannel(const MidiMessage& message) const
{
  return message.channel == patch_.midi_channel;
}

void FantomeEngine::HandleNoteOn(std::uint8_t note, std::uint8_t velocity)
{
  if (velocity == 0) {
    HandleNoteOff(note);
    return;
  }

  allocator_.NoteOn(note, velocity, patch_.play_mode);
}

void FantomeEngine::HandleNoteOff(std::uint8_t note)
{
  allocator_.NoteOff(note, patch_.play_mode);
}

void FantomeEngine::HandleControlChange(std::uint8_t controller, std::uint8_t value)
{
  switch (controller) {
    case kCcModWheel:
      performance_.mod_wheel = NormalizeMidi7(value);
      break;
    case kCcMasterVolume:
      patch_.master_volume = NormalizeMidi7(value);
      break;
    case kCcDelayFeedback:
      patch_.delay.feedback = NormalizeMidi7(value) * 0.95f;
      break;
    case kCcDelayMix:
      patch_.delay.mix = NormalizeMidi7(value);
      break;
    case kCcSustain:
      performance_.sustain = value >= 64;
      allocator_.SetSustain(performance_.sustain, patch_.play_mode);
      break;
    case kCcResonance:
      patch_.filter.resonance = NormalizeMidi7(value);
      break;
    case kCcCutoff:
      patch_.filter.cutoff = NormalizeMidi7(value);
      break;
    case kCcReverbMix:
      patch_.reverb.mix = NormalizeMidi7(value);
      break;
    case kCcChorusDepth:
      patch_.chorus.depth = NormalizeMidi7(value);
      break;
    case kCcAllNotesOff:
      allocator_.AllNotesOff();
      performance_.sustain = false;
      break;
    default:
      break;
  }
}

void FantomeEngine::HandleProgramChange(std::uint8_t program)
{
  if (program >= preset_bank_.size()) {
    return;
  }

  LoadPreset(program);
}

void FantomeEngine::HandlePitchBend(int value)
{
  constexpr float kPitchBendMax = 8192.0f;
  performance_.pitch_bend = std::clamp(static_cast<float>(value) / kPitchBendMax, -1.0f, 1.0f);
}

float FantomeEngine::NormalizeMidi7(std::uint8_t value)
{
  return static_cast<float>(value) / 127.0f;
}

}  // namespace fantome

