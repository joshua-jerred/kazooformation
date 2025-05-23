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

#include "common/assert.hpp"

namespace kazoo {

class IAudioChannel {
 public:
  using Sample = int16_t;

  virtual ~IAudioChannel() = default;

  virtual void addSample(int16_t sample) = 0;

  virtual const std::span<const Sample> getSamplesRef() const = 0;

  virtual void clear() = 0;
};

class AudioChannel : public IAudioChannel {
 public:
  AudioChannel() = default;

  void addSample(const Sample sample) override {
    samples_.push_back(sample);
  }

  void addSamples(std::span<const Sample> samples) {
    for (const auto sample : samples) {
      samples_.push_back(sample);
    }
  }

  size_t getNumSamples() const {
    return samples_.size();
  }

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

  void clear() override {
    samples_.clear();
  }

  uint32_t getLengthMs() const {
    return static_cast<uint32_t>(samples_.size() * 1000 / 44100);
  }

  Sample getSample(size_t index) const {
    if (index >= samples_.size()) {
      KTL_ASSERT(false);
    }
    return samples_.at(index);
  }

  void setSample(size_t index, Sample sample) {
    if (index >= samples_.size()) {
      KTL_ASSERT(false);
    }
    samples_.at(index) = sample;
  }

 protected:
  std::vector<Sample> samples_{};
};

}  // namespace kazoo