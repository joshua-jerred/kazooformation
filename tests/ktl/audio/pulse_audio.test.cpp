/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file pulse_audio_test.cpp
/// @date 2025-02-16

#include <testing.hpp>

#include <ktl/audio/audio_channel.hpp>
#include <ktl/audio/pulse_audio.hpp>

TEST(PulseAudio_test, player_test) {
  // Fill the channel with silence
  kazoo::AudioChannel channel;
  for (int i = 0; i < 1000; i++) {
    channel.addSample(0);
  }

  // Play the silence
  kazoo::PulseAudio::Player::play(channel);
}

TEST(PulseAudio_test, reader_test) {
  kazoo::PulseAudio::Reader reader;
  reader.process();

  // reader.getAudioBuffer();
}