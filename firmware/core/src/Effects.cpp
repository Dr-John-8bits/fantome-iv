#include "fantome/Effects.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>

namespace fantome {

namespace {

constexpr float kPi = 3.14159265358979323846f;
constexpr std::array<float, 5> kQuarterNoteSpans {4.0f, 2.0f, 1.0f, 0.5f, 0.25f};
constexpr std::array<int, 4> kCombTuningLeft {1116, 1188, 1277, 1356};
constexpr std::array<int, 4> kCombTuningRight {1139, 1211, 1300, 1379};
constexpr std::array<int, 2> kAllpassTuningLeft {556, 441};
constexpr std::array<int, 2> kAllpassTuningRight {579, 464};

float ClampSampleRate(float sample_rate)
{
  return std::clamp(sample_rate, 8000.0f, 192000.0f);
}

float ClockDivisionSeconds(int division_index, float tempo_bpm)
{
  const auto clamped_index = std::clamp(
    division_index,
    0,
    static_cast<int>(kQuarterNoteSpans.size() - 1));
  const auto quarter_note_span = kQuarterNoteSpans[static_cast<std::size_t>(clamped_index)];
  const auto quarter_note_seconds = 60.0f / std::max(tempo_bpm, 1.0f);
  return quarter_note_seconds * quarter_note_span;
}

float ClampMix(float mix)
{
  return std::clamp(mix, 0.0f, 1.0f);
}

std::size_t ScaleDelayLength(int base_length, float sample_rate)
{
  constexpr float kReferenceRate = 44100.0f;
  return static_cast<std::size_t>(
           std::max(1.0f, std::round((static_cast<float>(base_length) * sample_rate) / kReferenceRate))) +
         1u;
}

}  // namespace

void StereoDelayEffect::SetSampleRate(float sample_rate)
{
  sample_rate_ = ClampSampleRate(sample_rate);
  const auto max_seconds = 2.5f;
  const auto max_samples = static_cast<std::size_t>(std::ceil(sample_rate_ * max_seconds)) + 4u;
  left_buffer_.assign(max_samples, 0.0f);
  right_buffer_.assign(max_samples, 0.0f);
  write_index_ = 0;
}

void StereoDelayEffect::Reset()
{
  std::fill(left_buffer_.begin(), left_buffer_.end(), 0.0f);
  std::fill(right_buffer_.begin(), right_buffer_.end(), 0.0f);
  write_index_ = 0;
}

void StereoDelayEffect::Process(
  float& left,
  float& right,
  const DelaySettings& settings,
  float tempo_bpm)
{
  if (left_buffer_.empty() || right_buffer_.empty()) {
    return;
  }

  const auto mix = ClampMix(settings.mix);
  if (mix <= 0.0001f) {
    return;
  }

  const auto feedback = std::clamp(settings.feedback, 0.0f, 0.95f);
  const auto delay_seconds = DelayTimeSeconds(settings, tempo_bpm);
  const auto delay_samples = std::clamp(
    delay_seconds * sample_rate_,
    1.0f,
    static_cast<float>(left_buffer_.size() - 2));

  const auto delayed_left = ReadInterpolated(left_buffer_, delay_samples);
  const auto delayed_right = ReadInterpolated(right_buffer_, delay_samples);

  left_buffer_[write_index_] = std::tanh(left + (delayed_left * feedback));
  right_buffer_[write_index_] = std::tanh(right + (delayed_right * feedback));

  write_index_ = (write_index_ + 1) % left_buffer_.size();

  left = (left * (1.0f - mix)) + (delayed_left * mix);
  right = (right * (1.0f - mix)) + (delayed_right * mix);
}

float StereoDelayEffect::DelayTimeSeconds(
  const DelaySettings& settings,
  float tempo_bpm) const
{
  if (settings.sync_mode == SyncMode::MidiClock) {
    return ClockDivisionSeconds(settings.division_index, tempo_bpm);
  }

  const auto time_ms = std::clamp(settings.time_ms, 10.0f, 2000.0f);
  return time_ms / 1000.0f;
}

float StereoDelayEffect::ReadInterpolated(
  const std::vector<float>& buffer,
  float delay_samples) const
{
  const auto buffer_size = static_cast<float>(buffer.size());
  float read_position = static_cast<float>(write_index_) - delay_samples;
  while (read_position < 0.0f) {
    read_position += buffer_size;
  }

  const auto index_a = static_cast<std::size_t>(read_position) % buffer.size();
  const auto index_b = (index_a + 1u) % buffer.size();
  const auto frac = read_position - std::floor(read_position);
  return buffer[index_a] + ((buffer[index_b] - buffer[index_a]) * frac);
}

void StereoChorusEffect::SetSampleRate(float sample_rate)
{
  sample_rate_ = ClampSampleRate(sample_rate);
  const auto max_seconds = 0.08f;
  const auto max_samples = static_cast<std::size_t>(std::ceil(sample_rate_ * max_seconds)) + 8u;
  buffer_.assign(max_samples, 0.0f);
  write_index_ = 0;
}

void StereoChorusEffect::Reset()
{
  std::fill(buffer_.begin(), buffer_.end(), 0.0f);
  write_index_ = 0;
  phase_ = 0.0f;
}

void StereoChorusEffect::Process(float& left, float& right, const ChorusSettings& settings)
{
  if (buffer_.empty()) {
    return;
  }

  const auto mix = ClampMix(settings.mix);
  if (mix <= 0.0001f) {
    return;
  }

  const auto mono_input = 0.5f * (left + right);
  const auto base_delay_samples = sample_rate_ * 0.012f;
  const auto depth_samples = (2.0f + (settings.depth * 8.0f)) * (sample_rate_ / 1000.0f);
  const auto rate_hz = std::clamp(settings.rate_hz, 0.05f, 8.0f);

  const auto phase_left = std::sin(2.0f * kPi * phase_);
  const auto phase_right = std::sin((2.0f * kPi * phase_) + kPi);
  const auto phase_aux_left = std::sin((2.0f * kPi * phase_) + (0.5f * kPi));
  const auto phase_aux_right = std::sin((2.0f * kPi * phase_) + (1.5f * kPi));

  const auto left_delay_a = base_delay_samples + ((0.5f + (0.5f * phase_left)) * depth_samples);
  const auto right_delay_a = base_delay_samples + ((0.5f + (0.5f * phase_right)) * depth_samples);
  const auto left_delay_b = base_delay_samples * 1.35f + ((0.5f + (0.5f * phase_aux_left)) * depth_samples * 0.75f);
  const auto right_delay_b = base_delay_samples * 1.35f + ((0.5f + (0.5f * phase_aux_right)) * depth_samples * 0.75f);

  const auto wet_left = 0.5f * (ReadInterpolated(left_delay_a) + ReadInterpolated(left_delay_b));
  const auto wet_right = 0.5f * (ReadInterpolated(right_delay_a) + ReadInterpolated(right_delay_b));

  buffer_[write_index_] = mono_input;
  write_index_ = (write_index_ + 1) % buffer_.size();

  phase_ += rate_hz / sample_rate_;
  if (phase_ >= 1.0f) {
    phase_ -= std::floor(phase_);
  }

  left = (left * (1.0f - mix)) + (wet_left * mix);
  right = (right * (1.0f - mix)) + (wet_right * mix);
}

float StereoChorusEffect::ReadInterpolated(float delay_samples) const
{
  const auto buffer_size = static_cast<float>(buffer_.size());
  const auto clamped_delay = std::clamp(
    delay_samples,
    1.0f,
    static_cast<float>(buffer_.size() - 2));
  float read_position = static_cast<float>(write_index_) - clamped_delay;
  while (read_position < 0.0f) {
    read_position += buffer_size;
  }

  const auto index_a = static_cast<std::size_t>(read_position) % buffer_.size();
  const auto index_b = (index_a + 1u) % buffer_.size();
  const auto frac = read_position - std::floor(read_position);
  return buffer_[index_a] + ((buffer_[index_b] - buffer_[index_a]) * frac);
}

void StereoReverbEffect::SetSampleRate(float sample_rate)
{
  sample_rate_ = ClampSampleRate(sample_rate);

  for (std::size_t index = 0; index < kCombCount; ++index) {
    comb_left_[index].assign(ScaleDelayLength(kCombTuningLeft[index], sample_rate_), 0.0f);
    comb_right_[index].assign(ScaleDelayLength(kCombTuningRight[index], sample_rate_), 0.0f);
  }

  for (std::size_t index = 0; index < kAllpassCount; ++index) {
    allpass_left_[index].assign(ScaleDelayLength(kAllpassTuningLeft[index], sample_rate_), 0.0f);
    allpass_right_[index].assign(ScaleDelayLength(kAllpassTuningRight[index], sample_rate_), 0.0f);
  }

  Reset();
}

void StereoReverbEffect::Reset()
{
  for (auto& buffer : comb_left_) {
    std::fill(buffer.begin(), buffer.end(), 0.0f);
  }
  for (auto& buffer : comb_right_) {
    std::fill(buffer.begin(), buffer.end(), 0.0f);
  }
  for (auto& buffer : allpass_left_) {
    std::fill(buffer.begin(), buffer.end(), 0.0f);
  }
  for (auto& buffer : allpass_right_) {
    std::fill(buffer.begin(), buffer.end(), 0.0f);
  }

  comb_left_index_.fill(0);
  comb_right_index_.fill(0);
  allpass_left_index_.fill(0);
  allpass_right_index_.fill(0);
  comb_left_filter_.fill(0.0f);
  comb_right_filter_.fill(0.0f);
}

void StereoReverbEffect::Process(float& left, float& right, const ReverbSettings& settings)
{
  const auto mix = ClampMix(settings.mix);
  if (mix <= 0.0001f) {
    return;
  }

  const auto input = (left + right) * 0.020f;
  float wet_left = 0.0f;
  float wet_right = 0.0f;

  for (std::size_t index = 0; index < kCombCount; ++index) {
    wet_left += ProcessComb(
      input,
      comb_left_[index],
      comb_left_index_[index],
      comb_left_filter_[index]);
    wet_right += ProcessComb(
      input,
      comb_right_[index],
      comb_right_index_[index],
      comb_right_filter_[index]);
  }

  for (std::size_t index = 0; index < kAllpassCount; ++index) {
    wet_left = ProcessAllpass(wet_left, allpass_left_[index], allpass_left_index_[index]);
    wet_right = ProcessAllpass(wet_right, allpass_right_[index], allpass_right_index_[index]);
  }

  wet_left *= 0.35f;
  wet_right *= 0.35f;

  left = (left * (1.0f - mix)) + (wet_left * mix);
  right = (right * (1.0f - mix)) + (wet_right * mix);
}

float StereoReverbEffect::ProcessComb(
  float input,
  std::vector<float>& buffer,
  std::size_t& index,
  float& filter_store) const
{
  constexpr float kRoomSize = 0.78f;
  constexpr float kDamp = 0.28f;

  const auto output = buffer[index];
  filter_store = (output * (1.0f - kDamp)) + (filter_store * kDamp);
  buffer[index] = input + (filter_store * kRoomSize);
  index = (index + 1u) % buffer.size();
  return output;
}

float StereoReverbEffect::ProcessAllpass(
  float input,
  std::vector<float>& buffer,
  std::size_t& index) const
{
  constexpr float kFeedback = 0.5f;

  const auto buffered = buffer[index];
  const auto output = buffered - input;
  buffer[index] = input + (buffered * kFeedback);
  index = (index + 1u) % buffer.size();
  return output;
}

}  // namespace fantome
