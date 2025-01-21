/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file audio_channel.hpp
/// @date 2025-01-21

#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <span>

namespace kazoo {

class AudioChannel {
 public:
  using Sample = int16_t;

  AudioChannel() = default;

  void addSample(Sample sample) { samples_.push_back(sample); }

  void addSamples(std::span<const Sample> samples) {
    for (const auto sample : samples) {
      samples_.push_back(sample);
    }
  }

  size_t getNumSamples() const { return samples_.size(); }

  bool popSample(Sample& sample) {
    if (samples_.empty()) {
      return false;
    }

    sample = samples_.front();
    samples_.pop_front();
    return true;
  }

 protected:
  std::deque<Sample> samples_{};
};

}  // namespace kazoo