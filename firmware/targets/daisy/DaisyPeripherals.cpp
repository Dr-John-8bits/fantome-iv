#include "DaisyPeripherals.h"

#include <algorithm>

namespace fantome {

void DaisyControlAdcStub::Init()
{
  pending_frames_.clear();
}

bool DaisyControlAdcStub::Poll(RawHardwareInputFrame& input)
{
  if (pending_frames_.empty()) {
    return false;
  }

  const auto queued = pending_frames_.front();
  pending_frames_.erase(pending_frames_.begin());

  input.encoder_delta += queued.encoder_delta;
  for (std::size_t index = 0; index < input.buttons.size(); ++index) {
    input.buttons[index] = input.buttons[index] || queued.buttons[index];
  }
  for (std::size_t index = 0; index < input.pots.size(); ++index) {
    if (!queued.pot_available[index]) {
      continue;
    }

    input.pot_available[index] = true;
    input.pots[index] = queued.pots[index];
  }

  return true;
}

void DaisyControlAdcStub::QueueFrame(const RawHardwareInputFrame& input)
{
  pending_frames_.push_back(input);
}

void DaisyControlAdcStub::QueuePotRawSample(
  std::size_t pot_index,
  std::uint16_t raw_value,
  std::uint16_t full_scale)
{
  if (pot_index >= 8 || full_scale == 0) {
    return;
  }

  RawHardwareInputFrame frame;
  frame.pot_available[pot_index] = true;
  frame.pots[pot_index] =
    std::clamp(static_cast<float>(raw_value) / static_cast<float>(full_scale), 0.0f, 1.0f);
  QueueFrame(frame);
}

void DaisyMidiUartStub::Init()
{
  pending_messages_.clear();
  pending_bytes_.clear();
  ResetParser();
}

bool DaisyMidiUartStub::Drain(RawHardwareInputFrame& input)
{
  bool changed = false;
  while (!pending_messages_.empty() && input.midi_message_count < input.kMidiCapacity) {
    input.PushMidi(pending_messages_.front());
    pending_messages_.erase(pending_messages_.begin());
    changed = true;
  }

  MidiMessage parsed;
  while (input.midi_message_count < input.kMidiCapacity && TryParseNextMessage(parsed)) {
    input.PushMidi(parsed);
    changed = true;
  }

  return changed;
}

void DaisyMidiUartStub::QueueMessage(const MidiMessage& message)
{
  pending_messages_.push_back(message);
}

void DaisyMidiUartStub::QueueBytes(const std::vector<std::uint8_t>& bytes)
{
  pending_bytes_.insert(pending_bytes_.end(), bytes.begin(), bytes.end());
}

std::size_t DaisyMidiUartStub::PendingMessageCount() const
{
  return pending_messages_.size() + pending_bytes_.size();
}

bool DaisyMidiUartStub::TryParseNextMessage(MidiMessage& message)
{
  while (!pending_bytes_.empty()) {
    const auto byte = pending_bytes_.front();
    pending_bytes_.erase(pending_bytes_.begin());

    if (byte >= 0xF8) {
      switch (byte) {
        case 0xF8:
          message = MidiMessage::Clock();
          return true;
        case 0xFA:
          message = MidiMessage::Start();
          return true;
        case 0xFB:
          message = MidiMessage::Continue();
          return true;
        case 0xFC:
          message = MidiMessage::Stop();
          return true;
        default:
          continue;
      }
    }

    if ((byte & 0x80u) != 0u) {
      running_status_ = byte;
      data_bytes_.clear();
      continue;
    }

    if (running_status_ == 0) {
      continue;
    }

    data_bytes_.push_back(byte);
    const auto status_nibble = running_status_ & 0xF0u;
    const auto channel = static_cast<std::uint8_t>((running_status_ & 0x0Fu) + 1u);
    const auto needed_data_bytes = status_nibble == 0xC0u ? 1u : 2u;
    if (data_bytes_.size() < needed_data_bytes) {
      continue;
    }

    switch (status_nibble) {
      case 0x80u:
        message = MidiMessage::NoteOff(channel, data_bytes_[0], data_bytes_[1]);
        break;
      case 0x90u:
        message = MidiMessage::NoteOn(channel, data_bytes_[0], data_bytes_[1]);
        break;
      case 0xB0u:
        message = MidiMessage::ControlChange(channel, data_bytes_[0], data_bytes_[1]);
        break;
      case 0xC0u:
        message = MidiMessage::ProgramChange(channel, data_bytes_[0]);
        break;
      case 0xE0u: {
        const auto lsb = static_cast<int>(data_bytes_[0]);
        const auto msb = static_cast<int>(data_bytes_[1]);
        const auto bend = ((msb << 7) | lsb) - 8192;
        message = MidiMessage::PitchBend(channel, bend);
        break;
      }
      default:
        data_bytes_.clear();
        continue;
    }

    data_bytes_.clear();
    return true;
  }

  return false;
}

void DaisyMidiUartStub::ResetParser()
{
  running_status_ = 0;
  data_bytes_.clear();
}

void DaisyOledDisplayStub::Init()
{
  last_frame_ = OledTextFrame {};
  present_count_ = 0;
}

void DaisyOledDisplayStub::Present(const OledTextFrame& frame)
{
  last_frame_ = frame;
  ++present_count_;
}

const OledTextFrame& DaisyOledDisplayStub::LastFrame() const
{
  return last_frame_;
}

std::size_t DaisyOledDisplayStub::PresentCount() const
{
  return present_count_;
}

}  // namespace fantome
