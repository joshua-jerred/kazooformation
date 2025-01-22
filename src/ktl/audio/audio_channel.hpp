/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file audio_channel.hpp
/// @date 2025-01-21

#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <span>
#include <vector>

namespace kazoo {

class IAudioChannel {
 public:
  using Sample = int16_t;

  virtual ~IAudioChannel() = default;

  virtual void addSample(int16_t sample) = 0;

  virtual const std::span<const Sample> getSamplesRef() const = 0;
};

class AudioChannel : public IAudioChannel {
 public:
  AudioChannel() = default;

  void addSample(Sample sample) override { samples_.push_back(sample); }

  void addSamples(std::span<const Sample> samples) {
    for (const auto sample : samples) {
      samples_.push_back(sample);
    }
  }

  size_t getNumSamples() const { return samples_.size(); }

  // bool popSample(Sample& sample) {
  //   if (samples_.empty()) {
  //     return false;
  //   }
  //
  //   sample = samples_.front();
  //   samples_.pop_front();
  //   return true;
  // }

  const std::span<const Sample> getSamplesRef() const override {
    return {samples_.data(), samples_.size()};
  }

 protected:
  std::vector<Sample> samples_{};
};

}  // namespace kazoo