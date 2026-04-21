#pragma once

#include <array>
#include <cstddef>
#include <vector>

#include "fantome/Patch.h"

namespace fantome {

class StereoDelayEffect {
 public:
  void SetSampleRate(float sample_rate);
  void Reset();
  void Process(float& left, float& right, const DelaySettings& settings, float tempo_bpm);

 private:
  float DelayTimeSeconds(const DelaySettings& settings, float tempo_bpm) const;
  float ReadInterpolated(const std::vector<float>& buffer, float delay_samples) const;

  float sample_rate_ = 48000.0f;
  std::vector<float> left_buffer_ {};
  std::vector<float> right_buffer_ {};
  std::size_t write_index_ = 0;
};

class StereoChorusEffect {
 public:
  void SetSampleRate(float sample_rate);
  void Reset();
  void Process(float& left, float& right, const ChorusSettings& settings);

 private:
  float ReadInterpolated(float delay_samples) const;

  float sample_rate_ = 48000.0f;
  std::vector<float> buffer_ {};
  std::size_t write_index_ = 0;
  float phase_ = 0.0f;
};

class StereoReverbEffect {
 public:
  void SetSampleRate(float sample_rate);
  void Reset();
  void Process(float& left, float& right, const ReverbSettings& settings);

 private:
  static constexpr std::size_t kCombCount = 4;
  static constexpr std::size_t kAllpassCount = 2;

  float ProcessComb(
    float input,
    std::vector<float>& buffer,
    std::size_t& index,
    float& filter_store) const;
  float ProcessAllpass(
    float input,
    std::vector<float>& buffer,
    std::size_t& index) const;

  float sample_rate_ = 48000.0f;
  std::array<std::vector<float>, kCombCount> comb_left_ {};
  std::array<std::vector<float>, kCombCount> comb_right_ {};
  std::array<std::size_t, kCombCount> comb_left_index_ {};
  std::array<std::size_t, kCombCount> comb_right_index_ {};
  std::array<float, kCombCount> comb_left_filter_ {};
  std::array<float, kCombCount> comb_right_filter_ {};
  std::array<std::vector<float>, kAllpassCount> allpass_left_ {};
  std::array<std::vector<float>, kAllpassCount> allpass_right_ {};
  std::array<std::size_t, kAllpassCount> allpass_left_index_ {};
  std::array<std::size_t, kAllpassCount> allpass_right_index_ {};
};

}  // namespace fantome
