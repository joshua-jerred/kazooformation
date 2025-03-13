#include <iostream>

#include <ktl/translation_layer.hpp>

constexpr bool PRE_POST_PADDING = true;

void getUserInput(std::string &user_input_string) {
  std::cout << "Enter a string to be spoken by kazoo or 'q' to quit: ";
  std::getline(std::cin, user_input_string);
}

int main() {
  std::cout << "kazoo connect" << std::endl;

  std::string user_input;
  while (true) {
    getUserInput(user_input);
    if (user_input == "q") break;  // quit

    kazoo::TranslationLayer tl{kazoo::TranslationLayer::ModelType::K1_MODEL};
    std::span<const uint8_t> user_input_span{
        reinterpret_cast<const uint8_t *>(user_input.data()),
        user_input.size()};

    tl.addData(user_input_span);
    tl.encode(PRE_POST_PADDING);
    tl.playAudioBlocking();

    break;  // single input for testing
  }

  return 0;
}