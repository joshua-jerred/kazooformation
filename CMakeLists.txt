cmake_minimum_required(VERSION 3.17)

project(kazooformation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

### Kazoo Common Library
add_library(kazoo_common INTERFACE)
target_sources(kazoo_common INTERFACE
        src/common/assert.cpp
)

### KTL Library
add_library(kazoo_translation_layer INTERFACE)
target_sources(kazoo_translation_layer INTERFACE
        src/ktl/binary_stream.cpp
        src/ktl/translation_layer.cpp
        src/ktl/models/k1_model.cpp
)
target_include_directories(kazoo_translation_layer INTERFACE src)

add_subdirectory(lib/fftw-3.3.10 EXCLUDE_FROM_ALL)
target_link_libraries(kazoo_translation_layer INTERFACE kazoo_common fftw3 fftw3f)

### Pulse Audio
find_path(PULSEAUDIO_INCLUDE_DIR
NAMES pulse/pulseaudio.h
)
find_library(PULSEAUDIO_LIBRARY
NAMES pulse
)
include_directories(${PULSEAUDIO_INCLUDE_DIRS})
# target_link_libraries(SignalEasel PRIVATE pulse-simple pulse)
# target_compile_definitions(SignalEasel PRIVATE PULSE_AUDIO_ENABLED)

### Executables
add_executable(kazoo_connect
        src/main.cpp
)
target_link_libraries(kazoo_connect kazoo_translation_layer kazoo_common pulse-simple pulse)

# ==== Unit tests ====
enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

# lazy globbing (not best practice /shrug)
file(GLOB_RECURSE kazoo_translation_layer_test_sources "tests/ktl/*.test.cpp")
add_executable(kazoo_translation_layer_test ${kazoo_translation_layer_test_sources})
target_link_libraries(kazoo_translation_layer_test
        GTest::GTest
        GTest::Main
        kazoo_translation_layer
)
gtest_discover_tests(kazoo_translation_layer_test)
target_include_directories(kazoo_translation_layer_test
        PRIVATE
        tests
        tests/ktl
)
configure_file(tests/ktl/audio/wav_file.test.wav ${CMAKE_CURRENT_BINARY_DIR}/wav_file_input.test.wav COPYONLY)
