#include "fantome/PortableInput.h"

#include <algorithm>

namespace fantome {

ControlEvent ControlEvent::EncoderTurn(int delta)
{
  return ControlEvent {ControlEventType::EncoderTurn, delta, 0, 0.0f};
}

ControlEvent ControlEvent::EncoderPress()
{
  return ControlEvent {ControlEventType::EncoderPress, 0, 0, 0.0f};
}

ControlEvent ControlEvent::PagePrevious()
{
  return ControlEvent {ControlEventType::PagePrevious, 0, 0, 0.0f};
}

ControlEvent ControlEvent::PageNext()
{
  return ControlEvent {ControlEventType::PageNext, 0, 0, 0.0f};
}

ControlEvent ControlEvent::BackPress()
{
  return ControlEvent {ControlEventType::BackPress, 0, 0, 0.0f};
}

ControlEvent ControlEvent::ActionPress()
{
  return ControlEvent {ControlEventType::ActionPress, 0, 0, 0.0f};
}

ControlEvent ControlEvent::PotMove(std::size_t pot_index, float normalized)
{
  return ControlEvent {ControlEventType::PotMove, 0, pot_index, normalized};
}

PortableInputSurface::PortableInputSurface() = default;

void PortableInputSurface::Reset()
{
  pot_positions_.fill(0.0f);
  encoder_position_ = 0;
}

bool PortableInputSurface::Apply(
  const ControlEvent& event,
  FantomeEngine& engine,
  UiState& ui)
{
  switch (event.type) {
    case ControlEventType::EncoderTurn:
      TurnEncoder(event.delta, engine, ui);
      return true;
    case ControlEventType::EncoderPress:
      PressEncoder(engine, ui);
      return true;
    case ControlEventType::PagePrevious:
      PressPagePrevious(engine, ui);
      return true;
    case ControlEventType::PageNext:
      PressPageNext(engine, ui);
      return true;
    case ControlEventType::BackPress:
      PressBack(engine, ui);
      return true;
    case ControlEventType::ActionPress:
      PressAction(engine, ui);
      return true;
    case ControlEventType::PotMove:
      return MovePot(event.control_index, event.normalized, engine, ui);
  }

  return false;
}

void PortableInputSurface::TurnEncoder(int delta, FantomeEngine& engine, UiState& ui)
{
  encoder_position_ += delta;
  ui.RotateEncoder(delta, engine);
}

void PortableInputSurface::PressEncoder(FantomeEngine& engine, UiState& ui)
{
  ui.PressEncoder(engine);
}

void PortableInputSurface::PressPagePrevious(FantomeEngine& engine, UiState& ui)
{
  ui.PreviousPage(engine);
}

void PortableInputSurface::PressPageNext(FantomeEngine& engine, UiState& ui)
{
  ui.NextPage(engine);
}

void PortableInputSurface::PressBack(FantomeEngine& engine, UiState& ui)
{
  ui.PressBack(engine);
}

void PortableInputSurface::PressAction(FantomeEngine& engine, UiState& ui)
{
  ui.PressAction(engine);
}

bool PortableInputSurface::MovePot(
  std::size_t pot_index,
  float normalized,
  FantomeEngine& engine,
  UiState& ui)
{
  if (pot_index >= pot_positions_.size()) {
    return false;
  }

  pot_positions_[pot_index] = std::clamp(normalized, 0.0f, 1.0f);
  return ui.MovePot(pot_index, pot_positions_[pot_index], engine);
}

const std::array<float, 8>& PortableInputSurface::PotPositions() const
{
  return pot_positions_;
}

int PortableInputSurface::EncoderPosition() const
{
  return encoder_position_;
}

}  // namespace fantome
