#include "fantome/PatchSerialization.h"

#include <cstdint>

namespace fantome {

namespace {

bool ParseBool(const std::string& value, bool& output)
{
  if (value == "0") {
    output = false;
    return true;
  }
  if (value == "1") {
    output = true;
    return true;
  }
  return false;
}

bool ParseInt(const std::string& value, int& output)
{
  try {
    output = std::stoi(value);
    return true;
  } catch (...) {
    return false;
  }
}

bool ParseFloat(const std::string& value, float& output)
{
  try {
    output = std::stof(value);
    return true;
  } catch (...) {
    return false;
  }
}

std::string EnumToString(int value)
{
  return std::to_string(value);
}

}  // namespace

bool PatchSerialization::ApplyKeyValue(
  Patch& patch,
  const std::string& key,
  const std::string& value)
{
  int int_value = 0;
  float float_value = 0.0f;
  bool bool_value = false;

  if (key == "name") {
    patch.name = value;
    return true;
  }
  if (key == "play_mode" && ParseInt(value, int_value)) {
    patch.play_mode = static_cast<PlayMode>(int_value);
    return true;
  }
  if (key == "legato_enabled" && ParseBool(value, bool_value)) {
    patch.legato_enabled = bool_value;
    return true;
  }
  if (key == "midi_channel" && ParseInt(value, int_value)) {
    patch.midi_channel = static_cast<std::uint8_t>(int_value);
    return true;
  }
  if (key == "pitch_bend_range_semitones" && ParseInt(value, int_value)) {
    patch.pitch_bend_range_semitones = int_value;
    return true;
  }
  if (key == "master_volume" && ParseFloat(value, float_value)) {
    patch.master_volume = float_value;
    return true;
  }

  if (key == "osc_a.waveform" && ParseInt(value, int_value)) {
    patch.osc_a.waveform = static_cast<Waveform>(int_value);
    return true;
  }
  if (key == "osc_a.octave" && ParseInt(value, int_value)) {
    patch.osc_a.octave = int_value;
    return true;
  }
  if (key == "osc_a.fine_tune_cents" && ParseFloat(value, float_value)) {
    patch.osc_a.fine_tune_cents = float_value;
    return true;
  }
  if (key == "osc_a.level" && ParseFloat(value, float_value)) {
    patch.osc_a.level = float_value;
    return true;
  }
  if (key == "osc_a.pwm" && ParseFloat(value, float_value)) {
    patch.osc_a.pwm = float_value;
    return true;
  }
  if (key == "osc_a.sync_enabled" && ParseBool(value, bool_value)) {
    patch.osc_a.sync_enabled = bool_value;
    return true;
  }

  if (key == "osc_b.waveform" && ParseInt(value, int_value)) {
    patch.osc_b.waveform = static_cast<Waveform>(int_value);
    return true;
  }
  if (key == "osc_b.octave" && ParseInt(value, int_value)) {
    patch.osc_b.octave = int_value;
    return true;
  }
  if (key == "osc_b.fine_tune_cents" && ParseFloat(value, float_value)) {
    patch.osc_b.fine_tune_cents = float_value;
    return true;
  }
  if (key == "osc_b.level" && ParseFloat(value, float_value)) {
    patch.osc_b.level = float_value;
    return true;
  }
  if (key == "osc_b.pwm" && ParseFloat(value, float_value)) {
    patch.osc_b.pwm = float_value;
    return true;
  }
  if (key == "osc_b.sync_enabled" && ParseBool(value, bool_value)) {
    patch.osc_b.sync_enabled = bool_value;
    return true;
  }

  if (key == "noise_level" && ParseFloat(value, float_value)) {
    patch.noise_level = float_value;
    return true;
  }

  if (key == "filter.cutoff" && ParseFloat(value, float_value)) {
    patch.filter.cutoff = float_value;
    return true;
  }
  if (key == "filter.resonance" && ParseFloat(value, float_value)) {
    patch.filter.resonance = float_value;
    return true;
  }
  if (key == "filter.env_amount" && ParseFloat(value, float_value)) {
    patch.filter.env_amount = float_value;
    return true;
  }
  if (key == "filter.lfo_amount" && ParseFloat(value, float_value)) {
    patch.filter.lfo_amount = float_value;
    return true;
  }
  if (key == "filter.sample_hold_amount" && ParseFloat(value, float_value)) {
    patch.filter.sample_hold_amount = float_value;
    return true;
  }

  if (key == "amp_env.attack_s" && ParseFloat(value, float_value)) {
    patch.amp_env.attack_s = float_value;
    return true;
  }
  if (key == "amp_env.decay_s" && ParseFloat(value, float_value)) {
    patch.amp_env.decay_s = float_value;
    return true;
  }
  if (key == "amp_env.sustain" && ParseFloat(value, float_value)) {
    patch.amp_env.sustain = float_value;
    return true;
  }
  if (key == "amp_env.release_s" && ParseFloat(value, float_value)) {
    patch.amp_env.release_s = float_value;
    return true;
  }

  if (key == "filter_env.attack_s" && ParseFloat(value, float_value)) {
    patch.filter_env.attack_s = float_value;
    return true;
  }
  if (key == "filter_env.release_s" && ParseFloat(value, float_value)) {
    patch.filter_env.release_s = float_value;
    return true;
  }

  auto apply_modulation = [&](ModulationSettings& modulation, const std::string& prefix) {
    if (key == prefix + ".sync_mode" && ParseInt(value, int_value)) {
      modulation.sync_mode = static_cast<SyncMode>(int_value);
      return true;
    }
    if (key == prefix + ".waveform" && ParseInt(value, int_value)) {
      modulation.waveform = static_cast<Waveform>(int_value);
      return true;
    }
    if (key == prefix + ".rate_hz" && ParseFloat(value, float_value)) {
      modulation.rate_hz = float_value;
      return true;
    }
    if (key == prefix + ".amount" && ParseFloat(value, float_value)) {
      modulation.amount = float_value;
      return true;
    }
    if (key == prefix + ".division_index" && ParseInt(value, int_value)) {
      modulation.division_index = int_value;
      return true;
    }
    return false;
  };

  if (apply_modulation(patch.osc_lfo, "osc_lfo")) {
    return true;
  }
  if (apply_modulation(patch.filter_lfo, "filter_lfo")) {
    return true;
  }
  if (apply_modulation(patch.filter_sample_hold, "filter_sample_hold")) {
    return true;
  }

  if (key == "chorus.depth" && ParseFloat(value, float_value)) {
    patch.chorus.depth = float_value;
    return true;
  }
  if (key == "chorus.rate_hz" && ParseFloat(value, float_value)) {
    patch.chorus.rate_hz = float_value;
    return true;
  }
  if (key == "chorus.mix" && ParseFloat(value, float_value)) {
    patch.chorus.mix = float_value;
    return true;
  }

  if (key == "delay.sync_mode" && ParseInt(value, int_value)) {
    patch.delay.sync_mode = static_cast<SyncMode>(int_value);
    return true;
  }
  if (key == "delay.time_ms" && ParseFloat(value, float_value)) {
    patch.delay.time_ms = float_value;
    return true;
  }
  if (key == "delay.division_index" && ParseInt(value, int_value)) {
    patch.delay.division_index = int_value;
    return true;
  }
  if (key == "delay.feedback" && ParseFloat(value, float_value)) {
    patch.delay.feedback = float_value;
    return true;
  }
  if (key == "delay.mix" && ParseFloat(value, float_value)) {
    patch.delay.mix = float_value;
    return true;
  }

  if (key == "reverb.mix" && ParseFloat(value, float_value)) {
    patch.reverb.mix = float_value;
    return true;
  }

  return false;
}

void PatchSerialization::WriteKeyValues(std::ostream& output, const Patch& patch)
{
  output << "name=" << patch.name << '\n';
  output << "play_mode=" << EnumToString(static_cast<int>(patch.play_mode)) << '\n';
  output << "legato_enabled=" << (patch.legato_enabled ? 1 : 0) << '\n';
  output << "midi_channel=" << static_cast<int>(patch.midi_channel) << '\n';
  output << "pitch_bend_range_semitones=" << patch.pitch_bend_range_semitones << '\n';
  output << "master_volume=" << patch.master_volume << '\n';

  output << "osc_a.waveform=" << EnumToString(static_cast<int>(patch.osc_a.waveform)) << '\n';
  output << "osc_a.octave=" << patch.osc_a.octave << '\n';
  output << "osc_a.fine_tune_cents=" << patch.osc_a.fine_tune_cents << '\n';
  output << "osc_a.level=" << patch.osc_a.level << '\n';
  output << "osc_a.pwm=" << patch.osc_a.pwm << '\n';
  output << "osc_a.sync_enabled=" << (patch.osc_a.sync_enabled ? 1 : 0) << '\n';

  output << "osc_b.waveform=" << EnumToString(static_cast<int>(patch.osc_b.waveform)) << '\n';
  output << "osc_b.octave=" << patch.osc_b.octave << '\n';
  output << "osc_b.fine_tune_cents=" << patch.osc_b.fine_tune_cents << '\n';
  output << "osc_b.level=" << patch.osc_b.level << '\n';
  output << "osc_b.pwm=" << patch.osc_b.pwm << '\n';
  output << "osc_b.sync_enabled=" << (patch.osc_b.sync_enabled ? 1 : 0) << '\n';

  output << "noise_level=" << patch.noise_level << '\n';
  output << "filter.cutoff=" << patch.filter.cutoff << '\n';
  output << "filter.resonance=" << patch.filter.resonance << '\n';
  output << "filter.env_amount=" << patch.filter.env_amount << '\n';
  output << "filter.lfo_amount=" << patch.filter.lfo_amount << '\n';
  output << "filter.sample_hold_amount=" << patch.filter.sample_hold_amount << '\n';

  output << "amp_env.attack_s=" << patch.amp_env.attack_s << '\n';
  output << "amp_env.decay_s=" << patch.amp_env.decay_s << '\n';
  output << "amp_env.sustain=" << patch.amp_env.sustain << '\n';
  output << "amp_env.release_s=" << patch.amp_env.release_s << '\n';

  output << "filter_env.attack_s=" << patch.filter_env.attack_s << '\n';
  output << "filter_env.release_s=" << patch.filter_env.release_s << '\n';

  auto write_modulation = [&](const char* prefix, const ModulationSettings& modulation) {
    output << prefix << ".sync_mode=" << EnumToString(static_cast<int>(modulation.sync_mode))
           << '\n';
    output << prefix << ".waveform=" << EnumToString(static_cast<int>(modulation.waveform))
           << '\n';
    output << prefix << ".rate_hz=" << modulation.rate_hz << '\n';
    output << prefix << ".amount=" << modulation.amount << '\n';
    output << prefix << ".division_index=" << modulation.division_index << '\n';
  };

  write_modulation("osc_lfo", patch.osc_lfo);
  write_modulation("filter_lfo", patch.filter_lfo);
  write_modulation("filter_sample_hold", patch.filter_sample_hold);

  output << "chorus.depth=" << patch.chorus.depth << '\n';
  output << "chorus.rate_hz=" << patch.chorus.rate_hz << '\n';
  output << "chorus.mix=" << patch.chorus.mix << '\n';

  output << "delay.sync_mode=" << EnumToString(static_cast<int>(patch.delay.sync_mode)) << '\n';
  output << "delay.time_ms=" << patch.delay.time_ms << '\n';
  output << "delay.division_index=" << patch.delay.division_index << '\n';
  output << "delay.feedback=" << patch.delay.feedback << '\n';
  output << "delay.mix=" << patch.delay.mix << '\n';

  output << "reverb.mix=" << patch.reverb.mix << '\n';
}

}  // namespace fantome
