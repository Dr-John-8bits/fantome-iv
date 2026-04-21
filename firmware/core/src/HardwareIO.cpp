#include "fantome/HardwareIO.h"

#include <algorithm>
#include <cmath>

namespace fantome {

namespace {

constexpr std::size_t ToIndex(HardwareButtonId button)
{
  return static_cast<std::size_t>(button);
}

constexpr float kPotSmoothing = 0.35f;
constexpr float kPotChangeThreshold = 0.001f;

}  // namespace

bool HardwareInputFrame::PushMidi(const MidiMessage& message)
{
  if (midi_message_count >= midi_messages.size()) {
    return false;
  }

  midi_messages[midi_message_count++] = message;
  return true;
}

bool RawHardwareInputFrame::PushMidi(const MidiMessage& message)
{
  if (midi_message_count >= midi_messages.size()) {
    return false;
  }

  midi_messages[midi_message_count++] = message;
  return true;
}

void HardwareInputFrame::ClearTransient()
{
  encoder_delta = 0;
  midi_message_count = 0;
  for (auto& button : buttons) {
    button.just_pressed = false;
  }
}

void RawHardwareInputFrame::ClearTransient()
{
  encoder_delta = 0;
  midi_message_count = 0;
}

bool HardwareAudioBuffer::IsValid() const
{
  return left != nullptr && right != nullptr && frame_count > 0;
}

void HardwareAudioBuffer::Clear() const
{
  if (!IsValid()) {
    return;
  }

  std::fill(left, left + frame_count, 0.0f);
  std::fill(right, right + frame_count, 0.0f);
}

void HardwareControlScanner::Reset()
{
  previous_buttons_.fill(false);
  smoothed_pots_.fill(0.0f);
  has_smoothed_pot_.fill(false);
}

bool HardwareControlScanner::Scan(const RawHardwareInputFrame& raw, HardwareInputFrame& cooked)
{
  cooked.ClearTransient();
  cooked.encoder_delta = raw.encoder_delta;

  bool changed = raw.encoder_delta != 0 || raw.midi_message_count > 0;

  for (std::size_t index = 0; index < cooked.buttons.size(); ++index) {
    const auto pressed = raw.buttons[index];
    cooked.buttons[index].pressed = pressed;
    cooked.buttons[index].just_pressed = pressed && !previous_buttons_[index];
    changed |= cooked.buttons[index].just_pressed || (pressed != previous_buttons_[index]);
    previous_buttons_[index] = pressed;
  }

  for (std::size_t index = 0; index < cooked.pots.size(); ++index) {
    if (!raw.pot_available[index]) {
      continue;
    }

    const auto clamped = std::clamp(raw.pots[index], 0.0f, 1.0f);
    if (!has_smoothed_pot_[index]) {
      smoothed_pots_[index] = clamped;
      has_smoothed_pot_[index] = true;
      changed = true;
    } else {
      const auto before = smoothed_pots_[index];
      smoothed_pots_[index] += (clamped - smoothed_pots_[index]) * kPotSmoothing;
      changed |= std::fabs(smoothed_pots_[index] - before) >= kPotChangeThreshold;
    }

    cooked.pots[index].available = true;
    cooked.pots[index].normalized = smoothed_pots_[index];
  }

  for (std::size_t index = 0; index < raw.midi_message_count; ++index) {
    cooked.PushMidi(raw.midi_messages[index]);
  }

  return changed;
}

void HardwareControlRouter::Reset()
{
  input_surface_.Reset();
}

bool HardwareControlRouter::Apply(
  const HardwareInputFrame& frame,
  FantomeEngine& engine,
  UiState& ui)
{
  bool changed = false;

  if (frame.encoder_delta != 0) {
    input_surface_.TurnEncoder(frame.encoder_delta, engine, ui);
    changed = true;
  }

  if (frame.buttons[ToIndex(HardwareButtonId::Encoder)].just_pressed) {
    input_surface_.PressEncoder(engine, ui);
    changed = true;
  }
  if (frame.buttons[ToIndex(HardwareButtonId::PagePrevious)].just_pressed) {
    input_surface_.PressPagePrevious(engine, ui);
    changed = true;
  }
  if (frame.buttons[ToIndex(HardwareButtonId::PageNext)].just_pressed) {
    input_surface_.PressPageNext(engine, ui);
    changed = true;
  }
  if (frame.buttons[ToIndex(HardwareButtonId::Back)].just_pressed) {
    input_surface_.PressBack(engine, ui);
    changed = true;
  }
  if (frame.buttons[ToIndex(HardwareButtonId::Action)].just_pressed) {
    input_surface_.PressAction(engine, ui);
    changed = true;
  }

  for (std::size_t index = 0; index < frame.pots.size(); ++index) {
    if (!frame.pots[index].available) {
      continue;
    }

    changed |= input_surface_.MovePot(
      index,
      std::clamp(frame.pots[index].normalized, 0.0f, 1.0f),
      engine,
      ui);
  }

  return changed;
}

const PortableInputSurface& HardwareControlRouter::InputSurface() const
{
  return input_surface_;
}

}  // namespace fantome
