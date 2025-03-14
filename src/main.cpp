/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file main.cpp
/// @date 2025-03-13

#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <ktl/translation_layer.hpp>

constexpr bool PRE_POST_PADDING = true;

std::atomic<bool> s_run_flag{true};
std::atomic<bool> s_have_user_input{false};
static std::string s_user_input = "";

void getUserInput() {
  // while (s_run_flag) {
  std::cout << "> ";
  std::getline(std::cin, s_user_input);
  s_have_user_input = true;

  if (s_user_input == "q") {
    s_run_flag = false;
    // break;
  }
  // }
}

int main() {
  std::cout << "kazoo connect" << std::endl;

  std::thread user_input_thread(getUserInput);

  kazoo::TranslationLayer tl{kazoo::TranslationLayer::ModelType::K1_MODEL};
  tl.startListening();

  while (s_run_flag) {
    if (s_have_user_input) {
      user_input_thread.join();

      std::string user_input = s_user_input;
      std::cout << "Processing user input: " << user_input << std::endl;
      s_have_user_input = false;

      std::span<const uint8_t> user_input_span{
          reinterpret_cast<const uint8_t *>(user_input.data()),
          user_input.size()};

      tl.addData(user_input_span);
      tl.encode(PRE_POST_PADDING);
      tl.playAudioBlocking();

      // Restart the user input thread
      user_input_thread = std::thread(getUserInput);
    }

    std::cout << tl.getStats() << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  if (user_input_thread.joinable()) {
    user_input_thread.join();
  }

  tl.stopListening();

  return 0;
}