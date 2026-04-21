#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <vector>

#include "fantome/Patch.h"

namespace fantome {

struct VoiceState {
  bool active = false;
  bool held = false;
  std::uint8_t note = 0;
  std::uint8_t velocity = 0;
  std::uint64_t age = 0;
};

class VoiceAllocator {
 public:
  const std::array<VoiceState, kVoiceCount>& Voices() const;

  void SetSustain(bool enabled, PlayMode mode);
  void NoteOn(std::uint8_t note, std::uint8_t velocity, PlayMode mode);
  void NoteOff(std::uint8_t note, PlayMode mode);
  void AllNotesOff();

 private:
  struct HeldNoteState {
    std::uint8_t note = 0;
    std::uint8_t velocity = 0;
  };

  std::optional<HeldNoteState> CurrentHeldNote() const;
  std::optional<std::size_t> FindFreeVoice() const;
  std::size_t FindOldestVoice() const;
  std::optional<std::size_t> FindNewestActiveVoiceForNote(std::uint8_t note) const;
  void ActivateVoice(std::size_t voice_index, std::uint8_t note, std::uint8_t velocity, bool held);
  void ApplyMonophonicNote(std::uint8_t note, std::uint8_t velocity, bool unison);
  void RefreshMonophonicState(PlayMode mode);
  void RegisterHeldNote(std::uint8_t note, std::uint8_t velocity);
  void ReleaseHeldNote(std::uint8_t note);
  void ReleaseSustainedPolyVoices();

  std::array<VoiceState, kVoiceCount> voices_ {};
  std::vector<HeldNoteState> held_notes_ {};
  bool sustain_enabled_ = false;
  std::uint64_t age_counter_ = 0;
};

}  // namespace fantome

