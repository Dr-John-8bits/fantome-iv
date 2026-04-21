#include "fantome/HardwareIO.h"

#include <algorithm>

namespace fantome {

namespace {

constexpr std::size_t ToIndex(HardwareButtonId button)
{
  return static_cast<std::size_t>(button);
}

}  // namespace

bool HardwareInputFrame::PushMidi(const MidiMessage& message)
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
