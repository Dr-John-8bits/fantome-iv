#include "DaisyTargetConfig.h"

#include <sstream>

namespace fantome {

const DaisyTargetConfig& DefaultDaisyTargetConfig()
{
  static const DaisyTargetConfig config {
    DaisyAudioConfig {48000.0f, 48},
    DaisyEncoderTargetConfig {},
    {{
      DaisyButtonTargetConfig {
        HardwareButtonId::PagePrevious,
        "BTN_PAGE_PREV",
        4,
        "D3",
        "PC9",
        true,
        true,
      },
      DaisyButtonTargetConfig {
        HardwareButtonId::PageNext,
        "BTN_PAGE_NEXT",
        5,
        "D4",
        "PC8",
        true,
        true,
      },
      DaisyButtonTargetConfig {
        HardwareButtonId::Back,
        "BTN_SHIFT",
        6,
        "D5",
        "PD2",
        true,
        true,
      },
      DaisyButtonTargetConfig {
        HardwareButtonId::Action,
        "BTN_ACTION",
        7,
        "D6",
        "PC12",
        true,
        true,
      },
    }},
    DaisyLedTargetConfig {},
    {{
      DaisyPotTargetConfig {0, "POT_1_MASTER", 22, "A0/D15", "PC0"},
      DaisyPotTargetConfig {1, "POT_2_CUTOFF", 23, "A1/D16", "PA3"},
      DaisyPotTargetConfig {2, "POT_3_RESO", 24, "A2/D17", "PB1"},
      DaisyPotTargetConfig {3, "POT_4_AMP_A", 25, "A3/D18", "PA7"},
      DaisyPotTargetConfig {4, "POT_5_AMP_D", 26, "A4/D19", "PA6"},
      DaisyPotTargetConfig {5, "POT_6_AMP_S", 27, "A5/D20", "PC1"},
      DaisyPotTargetConfig {6, "POT_7_AMP_R", 28, "A6/D21", "PC4"},
      DaisyPotTargetConfig {7, "POT_8_CONTEXT", 29, "A7/D22", "PA5"},
    }},
    DaisyOledTargetConfig {},
    DaisyMidiTargetConfig {},
    DaisySessionTargetConfig {},
  };
  return config;
}

std::string DaisyTargetConfigSummary(const DaisyTargetConfig& config)
{
  std::ostringstream summary;
  summary << "sr=" << config.audio.sample_rate
          << " block=" << config.audio.block_size
          << " enc=" << config.encoder.a_pin_label << "/" << config.encoder.b_pin_label
          << "/" << config.encoder.sw_pin_label
          << " btn=" << config.buttons.front().pin_label << ".."
          << config.buttons.back().pin_label
          << " pots=" << config.pots.front().pin_label << ".."
          << config.pots.back().pin_label
          << " oled=" << config.oled.scl_pin_label << "/" << config.oled.sda_pin_label
          << "@0x" << std::hex << static_cast<int>(config.oled.i2c_address)
          << std::dec
          << " midi=" << config.midi.rx_pin_label
          << "@" << config.midi.baud
          << " led=" << config.midi_led.pin_label
          << " session=" << config.session.boot_policy;
  return summary.str();
}

}  // namespace fantome
