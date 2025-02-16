/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file pulse_audio.hpp
/// @date 2025-02-02

#pragma once

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <pulse/error.h>
#include <pulse/simple.h>
#include <string>
#include <vector>

#include <common/assert.hpp>
#include <ktl/audio/audio_channel.hpp>
#include <ktl/audio/audio_constants.hpp>

namespace kazoo {

namespace PulseAudio {

inline constexpr pa_sample_format_t PULSE_AUDIO_SAMPLE_FORMAT = PA_SAMPLE_S16NE;
inline constexpr uint8_t NUM_PULSE_CHANNELS = 1;
inline constexpr pa_sample_spec PULSE_AUDIO_SAMPLE_SPEC = {
    PULSE_AUDIO_SAMPLE_FORMAT, AUDIO_SAMPLE_RATE, NUM_PULSE_CHANNELS};
const std::string PULSE_AUDIO_APP_NAME = "ktl";
// inline constexpr size_t BUFFER_SIZE_IN_SECONDS = 1;
// inline constexpr size_t PULSE_AUDIO_BUFFER_SIZE = 16000;

/// @cite
/// https://github.com/joshua-jerred/SignalEasel/blob/main/src/pulse_audio_writer.cpp
/// @todo refactor
class Player {
 public:
  Player() = default;

  static void play(const IAudioChannel &channel) {
    const auto data = channel.getSamplesRef();

    pa_simple *s =
        pa_simple_new(nullptr,                       // Use the default server.
                      PULSE_AUDIO_APP_NAME.c_str(),  // Our application's name.
                      PA_STREAM_PLAYBACK,        // Stream direction (output).
                      nullptr,                   // Use the default device.
                      "StreamWriter",            // Description of our stream.
                      &PULSE_AUDIO_SAMPLE_SPEC,  // The output sample format
                      nullptr,                   // Use default channel map
                      nullptr,  // Use default buffering attributes.
                      nullptr   // Ignore error code.
        );

    if (!s) {
      KTL_ASSERT(false);
    }

    if (pa_simple_write(s, data.data(),
                        static_cast<size_t>(data.size() * sizeof(int16_t)),
                        nullptr) < 0) {
      KTL_ASSERT(false);
    }

    int res = pa_simple_drain(s, nullptr);
    if (res < 0) {
      KTL_ASSERT(false);
    }

    pa_simple_free(s);
  }
};

// typedef std::array<int16_t, PULSE_AUDIO_BUFFER_SIZE> PulseAudioBuffer;

// class Reader {
//  public:
//   Reader();
//   ~Reader();

//   // rule of 5
//   Reader(const Reader &) = delete;
//   Reader &operator=(const Reader &) = delete;
//   Reader(Reader &&) = delete;
//   Reader &operator=(Reader &&) = delete;

//   /**
//    * @brief Returns true if there was enough audio to process
//    */
//   bool process();
//   uint32_t getRms() const { return rms_; }
//   double getVolume() const { return volume_; }
//   uint64_t getLatency() const { return latency_; }

//   const PulseAudioBuffer &getAudioBuffer() const { return audio_buffer_; }

//  private:
//   /**
//    * @brief Latency in milliseconds
//    */
//   uint32_t latency_ = 0;
//   uint64_t rms_ = 0;
//   double volume_ = 0;
//   pa_sample_spec ss_ = {PULSE_AUDIO_SAMPLE_FORMAT, AUDIO_SAMPLE_RATE,
//                         NUM_PULSE_CHANNELS};

//   pa_simple *s_ = nullptr;

//   std::array<int16_t, PULSE_AUDIO_BUFFER_SIZE> audio_buffer_{};
// };

}  // namespace PulseAudio

}  // namespace kazoo