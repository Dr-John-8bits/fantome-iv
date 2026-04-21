#include "fantome/FantomeEngine.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "fantome/PresetPersistence.h"

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
constexpr std::array<float, kVoiceCount> kPolyVoicePan {-0.24f, -0.08f, 0.08f, 0.24f};
constexpr std::array<float, kVoiceCount> kUnisonVoicePan {-0.58f, -0.20f, 0.20f, 0.58f};

float SoftClip(float input)
{
  return std::tanh(input);
}

float VoicePanForMode(PlayMode mode, std::size_t voice_index)
{
  if (mode == PlayMode::Mono) {
    return 0.0f;
  }

  if (mode == PlayMode::Unison) {
    return kUnisonVoicePan[voice_index];
  }

  return kPolyVoicePan[voice_index];
}

float VoiceNormalizationForMode(PlayMode mode, std::size_t active_voice_count)
{
  const auto safe_count = static_cast<float>(std::max<std::size_t>(1, active_voice_count));
  const auto base = mode == PlayMode::Unison ? 1.02f : 0.88f;
  return base / std::sqrt(safe_count);
}

}  // namespace

FantomeEngine::FantomeEngine()
{
  Reset();
}

void FantomeEngine::Reset()
{
  preset_bank_ = MakeFactoryPresetBank();
  patch_ = preset_bank_[0];
  active_preset_slot_ = 0;
  allocator_.AllNotesOff();
  allocator_snapshot_ = allocator_.Voices();
  performance_ = PerformanceState {};
  transport_ = TransportState {};
  ResetDspVoices();
}

void FantomeEngine::SetSampleRate(float sample_rate)
{
  sample_rate_ = std::clamp(sample_rate, 8000.0f, 192000.0f);
  for (auto& voice : dsp_voices_) {
    voice.SetSampleRate(sample_rate_);
  }
  chorus_.SetSampleRate(sample_rate_);
  delay_.SetSampleRate(sample_rate_);
  reverb_.SetSampleRate(sample_rate_);
  osc_lfo_source_.SetSampleRate(sample_rate_);
  filter_lfo_source_.SetSampleRate(sample_rate_);
  filter_sample_hold_source_.SetSampleRate(sample_rate_);
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

  SyncVoicesFromAllocator();
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
  active_preset_slot_ = slot;
}

bool FantomeEngine::LoadPreset(std::size_t slot)
{
  if (slot >= preset_bank_.size()) {
    return false;
  }

  patch_ = preset_bank_[slot];
  active_preset_slot_ = slot;
  allocator_.AllNotesOff();
  allocator_snapshot_ = allocator_.Voices();
  ResetDspVoices();
  performance_.sustain = false;
  return true;
}

void FantomeEngine::InitializeCurrentPatch()
{
  patch_ = MakeInitPatch("Init");
  allocator_.AllNotesOff();
  allocator_snapshot_ = allocator_.Voices();
  ResetDspVoices();
  performance_.sustain = false;
}

bool FantomeEngine::SavePresetBankToFile(const std::string& path) const
{
  return PresetPersistence::SaveBank(path, preset_bank_, active_preset_slot_);
}

bool FantomeEngine::LoadPresetBankFromFile(const std::string& path)
{
  std::size_t active_slot = 0;
  if (!PresetPersistence::LoadBank(path, preset_bank_, active_slot)) {
    return false;
  }

  return LoadPreset(active_slot);
}

void FantomeEngine::Render(float* left, float* right, std::size_t frame_count)
{
  if (left == nullptr || right == nullptr || frame_count == 0) {
    return;
  }

  constexpr float kHeadroom = 0.22f;
  for (std::size_t frame = 0; frame < frame_count; ++frame) {
    last_modulation_ = BuildModulationFrame();
    const auto active_voice_count = static_cast<std::size_t>(std::count_if(
      dsp_voices_.begin(),
      dsp_voices_.end(),
      [](const SynthVoice& voice) {
        return voice.IsActive();
      }));
    const auto voice_gain = VoiceNormalizationForMode(patch_.play_mode, active_voice_count);
    float mix_left = 0.0f;
    float mix_right = 0.0f;

    for (std::size_t voice_index = 0; voice_index < dsp_voices_.size(); ++voice_index) {
      auto& voice = dsp_voices_[voice_index];
      const auto sample = voice.Render(
        patch_,
        last_modulation_,
        performance_.pitch_bend);
      const auto pan = VoicePanForMode(patch_.play_mode, voice_index);
      const auto gain_left = std::sqrt(0.5f * (1.0f - pan));
      const auto gain_right = std::sqrt(0.5f * (1.0f + pan));

      mix_left += sample * gain_left * voice_gain;
      mix_right += sample * gain_right * voice_gain;
    }

    mix_left = SoftClip(mix_left * 0.92f);
    mix_right = SoftClip(mix_right * 0.92f);

    chorus_.Process(mix_left, mix_right, patch_.chorus);
    delay_.Process(mix_left, mix_right, patch_.delay, transport_.tempo_bpm);
    reverb_.Process(mix_left, mix_right, patch_.reverb);

    left[frame] = SoftClip(mix_left * patch_.master_volume * kHeadroom);
    right[frame] = SoftClip(mix_right * patch_.master_volume * kHeadroom);
  }
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

const ModulationFrame& FantomeEngine::LastModulationFrame() const
{
  return last_modulation_;
}

float FantomeEngine::SampleRate() const
{
  return sample_rate_;
}

std::size_t FantomeEngine::CurrentPresetSlot() const
{
  return active_preset_slot_;
}

EngineSessionState FantomeEngine::ExportSessionState() const
{
  return EngineSessionState {
    patch_,
    preset_bank_,
    active_preset_slot_,
    transport_.tempo_bpm,
  };
}

bool FantomeEngine::RestoreSessionState(const EngineSessionState& state)
{
  if (state.active_preset_slot >= preset_bank_.size()) {
    return false;
  }

  preset_bank_ = state.preset_bank;
  patch_ = state.current_patch;
  active_preset_slot_ = state.active_preset_slot;
  transport_ = TransportState {};
  transport_.tempo_bpm = std::clamp(state.tempo_bpm, 30.0f, 300.0f);
  allocator_.AllNotesOff();
  allocator_snapshot_ = allocator_.Voices();
  performance_ = PerformanceState {};
  ResetDspVoices();
  return true;
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
      ResetDspVoices();
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

void FantomeEngine::SyncVoicesFromAllocator()
{
  const auto& allocator_voices = allocator_.Voices();

  for (std::size_t index = 0; index < allocator_voices.size(); ++index) {
    const auto& state = allocator_voices[index];
    const auto& previous = allocator_snapshot_[index];
    auto& dsp_voice = dsp_voices_[index];

    if (state.active) {
      const auto assignment_changed =
        !previous.active || state.note != previous.note || state.age != previous.age;

      if (assignment_changed) {
        bool retrigger = true;
        if ((patch_.play_mode == PlayMode::Mono || patch_.play_mode == PlayMode::Unison) &&
            patch_.legato_enabled &&
            previous.active) {
          retrigger = false;
        }

        dsp_voice.Start(state.note, state.velocity, retrigger);
      }
    } else if (previous.active) {
      dsp_voice.Release();
    }
  }

  allocator_snapshot_ = allocator_voices;
}

void FantomeEngine::ResetDspVoices()
{
  for (std::size_t index = 0; index < dsp_voices_.size(); ++index) {
    auto& voice = dsp_voices_[index];
    voice.SetVoiceIndex(index);
    voice.SetSampleRate(sample_rate_);
    voice.Reset();
  }

  chorus_.SetSampleRate(sample_rate_);
  chorus_.Reset();
  delay_.SetSampleRate(sample_rate_);
  delay_.Reset();
  reverb_.SetSampleRate(sample_rate_);
  reverb_.Reset();
  osc_lfo_source_.SetSampleRate(sample_rate_);
  osc_lfo_source_.Reset();
  filter_lfo_source_.SetSampleRate(sample_rate_);
  filter_lfo_source_.Reset();
  filter_sample_hold_source_.SetSampleRate(sample_rate_);
  filter_sample_hold_source_.Reset();
  last_modulation_ = ModulationFrame {};
}

ModulationFrame FantomeEngine::BuildModulationFrame()
{
  ModulationFrame frame;

  const auto osc_lfo_sample = osc_lfo_source_.Render(patch_.osc_lfo, transport_.tempo_bpm);
  const auto filter_lfo_sample = filter_lfo_source_.Render(patch_.filter_lfo, transport_.tempo_bpm);
  const auto sample_hold_sample = filter_sample_hold_source_.Render(
    patch_.filter_sample_hold,
    transport_.tempo_bpm);

  const auto osc_amount = patch_.osc_lfo.amount + (performance_.mod_wheel * 0.35f);
  const auto filter_lfo_amount = std::max(patch_.filter.lfo_amount, patch_.filter_lfo.amount);
  const auto filter_sample_hold_amount = std::max(
    patch_.filter.sample_hold_amount,
    patch_.filter_sample_hold.amount);

  frame.osc_pitch_semitones = osc_lfo_sample * osc_amount;
  frame.filter_cutoff_delta =
    (filter_lfo_sample * filter_lfo_amount * 0.35f) +
    (sample_hold_sample * filter_sample_hold_amount * 0.30f);

  return frame;
}

}  // namespace fantome
