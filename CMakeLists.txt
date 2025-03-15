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
        src/ktl/audio/fft.cpp
        src/ktl/binary_stream.cpp
        src/ktl/translation_layer.cpp
        src/ktl/models/k1_model.cpp
        src/ktl/models/k2/k2_model.cpp
        src/ktl/models/k2/kazoo_A_pcm.cpp
        src/ktl/models/k2/kazoo_D_pcm.cpp
        src/ktl/models/k2/kazoo_E_pcm.cpp
        src/ktl/models/k2/kazoo_G_pcm.cpp
        src/ktl/audio/kazoo_pcm/doot1_pcm.cpp
        src/ktl/audio/kazoo_pcm/doot2_pcm.cpp
        src/ktl/audio/kazoo_pcm/doot3_pcm.cpp
        src/ktl/audio/kazoo_pcm/doot4_pcm.cpp
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

# ============================= #
# == Executables ============== #
# ============================= #
add_executable(kazoo_connect
        src/main.cpp
)
target_link_libraries(kazoo_connect kazoo_translation_layer kazoo_common pulse-simple pulse)

add_executable(model_score
        "tests/ktl_model_scoring/main.cpp"
        "tests/ktl_model_scoring/model_tester.cpp"
)
target_link_libraries(model_score kazoo_translation_layer kazoo_common pulse-simple pulse)


# ============================= #
# == Unit tests =============== #
# ============================= #

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

set(kazoo_translation_layer_test_sources
        "tests/ktl/audio/fft.test.cpp"
        "tests/ktl/audio/pulse_audio.test.cpp"
        "tests/ktl/audio/wav_file.test.cpp"
        "tests/ktl/model/binary_model.test.cpp"
        "tests/ktl/model/k1_model.test.cpp"
        "tests/ktl/model/k2_peak_model.test.cpp"
        "tests/ktl/binary_stream.test.cpp"
        "tests/ktl/deframer.test.cpp"
        "tests/ktl/translation_layer.test.cpp"
)
add_executable(kazoo_translation_layer_test ${kazoo_translation_layer_test_sources})
target_link_libraries(kazoo_translation_layer_test
        GTest::GTest
        GTest::Main
        kazoo_translation_layer
        pulse-simple
        pulse
)
gtest_discover_tests(kazoo_translation_layer_test)
target_include_directories(kazoo_translation_layer_test
        PRIVATE
        tests
        tests/ktl
)
configure_file(tests/ktl/audio/kazooA_sample.test.wav ${CMAKE_CURRENT_BINARY_DIR}/kazooA_sample.test.wav COPYONLY)
configure_file(tests/ktl/audio/wav_file.test.wav ${CMAKE_CURRENT_BINARY_DIR}/wav_file_input.test.wav COPYONLY)
