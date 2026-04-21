#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "fantome/HardwareIO.h"

namespace fantome {

struct DaisyAudioConfig {
  float sample_rate = 48000.0f;
  std::size_t block_size = 48;
};

class DaisyControlAdc {
 public:
  virtual ~DaisyControlAdc() = default;

  virtual void Init() = 0;
  virtual bool Poll(RawHardwareInputFrame& input) = 0;
};

class DaisyMidiUart {
 public:
  virtual ~DaisyMidiUart() = default;

  virtual void Init() = 0;
  virtual bool Drain(RawHardwareInputFrame& input) = 0;
};

class DaisyOledDisplay {
 public:
  virtual ~DaisyOledDisplay() = default;

  virtual void Init() = 0;
  virtual void Present(const OledTextFrame& frame) = 0;
};

class DaisyControlAdcStub final : public DaisyControlAdc {
 public:
  void Init() override;
  bool Poll(RawHardwareInputFrame& input) override;

  void QueueFrame(const RawHardwareInputFrame& input);
  void QueuePotRawSample(
    std::size_t pot_index,
    std::uint16_t raw_value,
    std::uint16_t full_scale = 4095);

 private:
  std::vector<RawHardwareInputFrame> pending_frames_ {};
};

class DaisyMidiUartStub final : public DaisyMidiUart {
 public:
  void Init() override;
  bool Drain(RawHardwareInputFrame& input) override;

  void QueueMessage(const MidiMessage& message);
  void QueueBytes(const std::vector<std::uint8_t>& bytes);
  std::size_t PendingMessageCount() const;

 private:
  bool TryParseNextMessage(MidiMessage& message);
  void ResetParser();

  std::vector<MidiMessage> pending_messages_ {};
  std::vector<std::uint8_t> pending_bytes_ {};
  std::uint8_t running_status_ = 0;
  std::vector<std::uint8_t> data_bytes_ {};
};

class DaisyOledDisplayStub final : public DaisyOledDisplay {
 public:
  void Init() override;
  void Present(const OledTextFrame& frame) override;

  const OledTextFrame& LastFrame() const;
  std::size_t PresentCount() const;

 private:
  OledTextFrame last_frame_ {};
  std::size_t present_count_ = 0;
};

}  // namespace fantome
