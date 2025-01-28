
#include <testing.hpp>

#include <ktl/audio/audio_channel.hpp>
#include <ktl/audio/wav_file.hpp>
#include <ktl/encoder.hpp>
#include <ktl/models/testing_model.hpp>

TEST(Encoder_test, encodeAvailableSymbols) {
  const auto model = kazoo::model::Testing::Model{};

  // Throw a few symbols in the stream
  kazoo::SymbolStream<kazoo::model::Testing::Token> s_stream{model};
  s_stream.addSymbol(kazoo::model::Testing::Token::SYMBOL_0);
  s_stream.addSymbol(kazoo::model::Testing::Token::SYMBOL_0);
  s_stream.addSymbol(kazoo::model::Testing::Token::SYMBOL_1);
  s_stream.addSymbol(kazoo::model::Testing::Token::SYMBOL_0);
  s_stream.addSymbol(kazoo::model::Testing::Token::SYMBOL_1);
  EXPECT_EQ(s_stream.getNumSymbols(), 5);

  // Encode the symbols into the audio buffer
  kazoo::Encoder<kazoo::model::Testing::Token> encoder{model};
  kazoo::AudioChannel audio_channel;
  EXPECT_EQ(encoder.encodeAvailableSymbols(s_stream, audio_channel), 5);
  EXPECT_EQ(s_stream.getNumSymbols(),
            0);  // Ensure the symbols were popped from the stream

  EXPECT_EQ(audio_channel.getNumSamples(),
            kazoo::model::Testing::SAMPLES_PER_SYMBOL * 5);

  kazoo::WavFile wav_file;
  wav_file.loadFromAudioChannel(audio_channel);
  wav_file.write("encoder_test_1.wav");
}
