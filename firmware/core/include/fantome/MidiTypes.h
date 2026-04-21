#pragma once

#include <cstdint>

namespace fantome {

enum class MidiMessageType {
  NoteOn,
  NoteOff,
  ControlChange,
  ProgramChange,
  PitchBend,
  Clock,
  Start,
  Stop,
  Continue,
  Unknown,
};

struct MidiMessage {
  MidiMessageType type = MidiMessageType::Unknown;
  std::uint8_t channel = 1;
  std::uint8_t data1 = 0;
  std::uint8_t data2 = 0;
  int value = 0;

  static MidiMessage NoteOn(
    std::uint8_t channel,
    std::uint8_t note,
    std::uint8_t velocity)
  {
    return MidiMessage {MidiMessageType::NoteOn, channel, note, velocity, 0};
  }

  static MidiMessage NoteOff(
    std::uint8_t channel,
    std::uint8_t note,
    std::uint8_t velocity = 0)
  {
    return MidiMessage {MidiMessageType::NoteOff, channel, note, velocity, 0};
  }

  static MidiMessage ControlChange(
    std::uint8_t channel,
    std::uint8_t controller,
    std::uint8_t value)
  {
    return MidiMessage {MidiMessageType::ControlChange, channel, controller, value, 0};
  }

  static MidiMessage ProgramChange(std::uint8_t channel, std::uint8_t program)
  {
    return MidiMessage {MidiMessageType::ProgramChange, channel, program, 0, 0};
  }

  static MidiMessage PitchBend(std::uint8_t channel, int bend)
  {
    return MidiMessage {MidiMessageType::PitchBend, channel, 0, 0, bend};
  }

  static MidiMessage Clock()
  {
    return MidiMessage {MidiMessageType::Clock, 0, 0, 0, 0};
  }

  static MidiMessage Start()
  {
    return MidiMessage {MidiMessageType::Start, 0, 0, 0, 0};
  }

  static MidiMessage Stop()
  {
    return MidiMessage {MidiMessageType::Stop, 0, 0, 0, 0};
  }

  static MidiMessage Continue()
  {
    return MidiMessage {MidiMessageType::Continue, 0, 0, 0, 0};
  }
};

}  // namespace fantome

