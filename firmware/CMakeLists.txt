cmake_minimum_required(VERSION 3.20)

project(FantomeIVFirmware LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(
  fantome_core
  core/src/Effects.cpp
  core/src/FantomeEngine.cpp
  core/src/Modulation.cpp
  core/src/OledView.cpp
  core/src/PatchSerialization.cpp
  core/src/PortableInput.cpp
  core/src/PresetPersistence.cpp
  core/src/SessionManager.cpp
  core/src/SessionPersistence.cpp
  core/src/StartupDisplay.cpp
  core/src/SynthVoice.cpp
  core/src/UiState.cpp
  core/src/VoiceAllocator.cpp
)

target_include_directories(
  fantome_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core/include
)

target_compile_options(
  fantome_core
  PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_executable(
  fantome_desktop_smoke
  targets/desktop/main.cpp
)

target_link_libraries(fantome_desktop_smoke PRIVATE fantome_core)

add_executable(
  fantome_tests
  tests/test_main.cpp
)

target_link_libraries(fantome_tests PRIVATE fantome_core)
