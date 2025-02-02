#include <iostream>

#include <ktl/translation_layer.hpp>

int main() {
  std::cout << "kazoo connect" << std::endl;

  // const std::array<uint8_t, 12> OUTPUT{'H', 'e', 'l', 'l', 'o', ' ',
  //  'W', 'o', 'r', 'l', 'd', '!'};

  std::string user_input;
  while (user_input != "q") {
    std::cout << "Enter a string to be spoken by kazoo or 'q' to quit: ";
    std::getline(std::cin, user_input);

    kazoo::TranslationLayer tl{kazoo::TranslationLayer::ModelType::K1_MODEL};
    std::span<const uint8_t> user_input_span{
        reinterpret_cast<const uint8_t*>(user_input.data()), user_input.size()};
    tl.addData(user_input_span);
    tl.encode();
    tl.playAudioBlocking();
  }

  return 0;
}