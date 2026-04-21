#pragma once

#include <array>
#include <cstddef>

#include "fantome/FantomeEngine.h"
#include "fantome/UiState.h"

namespace fantome {

enum class ControlEventType {
  EncoderTurn,
  EncoderPress,
  PagePrevious,
  PageNext,
  BackPress,
  ActionPress,
  PotMove,
};

struct ControlEvent {
  ControlEventType type = ControlEventType::EncoderPress;
  int delta = 0;
  std::size_t control_index = 0;
  float normalized = 0.0f;

  static ControlEvent EncoderTurn(int delta);
  static ControlEvent EncoderPress();
  static ControlEvent PagePrevious();
  static ControlEvent PageNext();
  static ControlEvent BackPress();
  static ControlEvent ActionPress();
  static ControlEvent PotMove(std::size_t pot_index, float normalized);
};

class PortableInputSurface {
 public:
  PortableInputSurface();

  void Reset();
  bool Apply(const ControlEvent& event, FantomeEngine& engine, UiState& ui);
  void TurnEncoder(int delta, FantomeEngine& engine, UiState& ui);
  void PressEncoder(FantomeEngine& engine, UiState& ui);
  void PressPagePrevious(FantomeEngine& engine, UiState& ui);
  void PressPageNext(FantomeEngine& engine, UiState& ui);
  void PressBack(FantomeEngine& engine, UiState& ui);
  void PressAction(FantomeEngine& engine, UiState& ui);
  bool MovePot(std::size_t pot_index, float normalized, FantomeEngine& engine, UiState& ui);

  const std::array<float, 8>& PotPositions() const;
  int EncoderPosition() const;

 private:
  std::array<float, 8> pot_positions_ {};
  int encoder_position_ = 0;
};

}  // namespace fantome
