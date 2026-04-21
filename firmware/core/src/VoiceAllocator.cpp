#include "fantome/VoiceAllocator.h"

#include <algorithm>

namespace fantome {

const std::array<VoiceState, kVoiceCount>& VoiceAllocator::Voices() const
{
  return voices_;
}

void VoiceAllocator::SetSustain(bool enabled, PlayMode mode)
{
  sustain_enabled_ = enabled;
  if (sustain_enabled_) {
    return;
  }

  if (mode == PlayMode::Poly) {
    ReleaseSustainedPolyVoices();
    return;
  }

  RefreshMonophonicState(mode);
}

void VoiceAllocator::NoteOn(std::uint8_t note, std::uint8_t velocity, PlayMode mode)
{
  RegisterHeldNote(note, velocity);

  if (mode == PlayMode::Mono) {
    ApplyMonophonicNote(note, velocity, false);
    return;
  }

  if (mode == PlayMode::Unison) {
    ApplyMonophonicNote(note, velocity, true);
    return;
  }

  const auto voice_index = FindFreeVoice().value_or(FindOldestVoice());
  ActivateVoice(voice_index, note, velocity, true);
}

void VoiceAllocator::NoteOff(std::uint8_t note, PlayMode mode)
{
  ReleaseHeldNote(note);

  if (mode == PlayMode::Mono || mode == PlayMode::Unison) {
    if (!sustain_enabled_) {
      RefreshMonophonicState(mode);
    }
    return;
  }

  const auto maybe_voice_index = FindNewestActiveVoiceForNote(note);
  if (!maybe_voice_index.has_value()) {
    return;
  }

  auto& voice = voices_[*maybe_voice_index];
  if (sustain_enabled_) {
    voice.held = false;
    return;
  }

  voice = VoiceState {};
}

void VoiceAllocator::AllNotesOff()
{
  held_notes_.clear();
  sustain_enabled_ = false;
  for (auto& voice : voices_) {
    voice = VoiceState {};
  }
}

std::optional<VoiceAllocator::HeldNoteState> VoiceAllocator::CurrentHeldNote() const
{
  if (held_notes_.empty()) {
    return std::nullopt;
  }
  return held_notes_.back();
}

std::optional<std::size_t> VoiceAllocator::FindFreeVoice() const
{
  for (std::size_t index = 0; index < voices_.size(); ++index) {
    if (!voices_[index].active) {
      return index;
    }
  }
  return std::nullopt;
}

std::size_t VoiceAllocator::FindOldestVoice() const
{
  std::size_t oldest_index = 0;
  auto oldest_age = voices_[0].age;

  for (std::size_t index = 1; index < voices_.size(); ++index) {
    if (voices_[index].age < oldest_age) {
      oldest_age = voices_[index].age;
      oldest_index = index;
    }
  }

  return oldest_index;
}

std::optional<std::size_t> VoiceAllocator::FindNewestActiveVoiceForNote(std::uint8_t note) const
{
  std::optional<std::size_t> match;
  std::uint64_t newest_age = 0;

  for (std::size_t index = 0; index < voices_.size(); ++index) {
    const auto& voice = voices_[index];
    if (!voice.active || voice.note != note) {
      continue;
    }

    if (!match.has_value() || voice.age >= newest_age) {
      match = index;
      newest_age = voice.age;
    }
  }

  return match;
}

void VoiceAllocator::ActivateVoice(
  std::size_t voice_index,
  std::uint8_t note,
  std::uint8_t velocity,
  bool held)
{
  auto& voice = voices_[voice_index];
  voice.active = true;
  voice.held = held;
  voice.note = note;
  voice.velocity = velocity;
  voice.age = ++age_counter_;
}

void VoiceAllocator::ApplyMonophonicNote(
  std::uint8_t note,
  std::uint8_t velocity,
  bool unison)
{
  ActivateVoice(0, note, velocity, true);

  for (std::size_t index = 1; index < voices_.size(); ++index) {
    if (unison) {
      ActivateVoice(index, note, velocity, true);
    } else {
      voices_[index] = VoiceState {};
    }
  }
}

void VoiceAllocator::RefreshMonophonicState(PlayMode mode)
{
  const auto current_note = CurrentHeldNote();
  if (current_note.has_value()) {
    ApplyMonophonicNote(
      current_note->note,
      current_note->velocity,
      mode == PlayMode::Unison);
    return;
  }

  for (auto& voice : voices_) {
    voice = VoiceState {};
  }
}

void VoiceAllocator::RegisterHeldNote(std::uint8_t note, std::uint8_t velocity)
{
  held_notes_.push_back(HeldNoteState {note, velocity});
}

void VoiceAllocator::ReleaseHeldNote(std::uint8_t note)
{
  const auto reverse_it = std::find_if(
    held_notes_.rbegin(),
    held_notes_.rend(),
    [note](const HeldNoteState& held_note) {
      return held_note.note == note;
    });

  if (reverse_it == held_notes_.rend()) {
    return;
  }

  held_notes_.erase(std::prev(reverse_it.base()));
}

void VoiceAllocator::ReleaseSustainedPolyVoices()
{
  for (auto& voice : voices_) {
    if (voice.active && !voice.held) {
      voice = VoiceState {};
    }
  }
}

}  // namespace fantome

