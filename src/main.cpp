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

constexpr bool CHAT_MODE = true;
constexpr bool PRE_POST_PADDING = true;

std::atomic<bool> s_run_flag{true};
std::atomic<bool> s_have_user_input{false};
static std::string s_user_input = "";

bool rx_mode = true;

kazoo::TranslationLayer tl{kazoo::TranslationLayer::ModelType::K2_PEAK_MODEL};

void getUserInput() {
  // while (s_run_flag) {
  std::cout << "> ";
  std::getline(std::cin, s_user_input);
  s_have_user_input = true;

  if (s_user_input == "q") {
    s_run_flag = false;
    // break;
  }
  if (s_user_input == "tx") {
    if (rx_mode) {
      rx_mode = false;
      std::cout << "rx mode off" << std::endl;
      tl.stopListening();
    } else {
      rx_mode = true;
      std::cout << "rx mode on" << std::endl;
      tl.startListening();
    }
  }
  // }
}

int main() {
  std::cout << "kazoo connect" << std::endl;

  std::thread user_input_thread(getUserInput);

  tl.startListening();

  while (s_run_flag) {
    if (s_have_user_input) {
      user_input_thread.join();

      std::string user_input = s_user_input;
      // std::cout << "Processing user input: " << user_input << std::endl;
      s_have_user_input = false;

      if (user_input == "q") {
        break;
      }

      // std::span<const uint8_t> user_input_span{
      // reinterpret_cast<const uint8_t *>(user_input.data()),
      // user_input.size()};

      if (user_input != "tx") {
        kazoo::KtlFrame frame{user_input};
        tl.sendFrame(frame);
        auto stats = tl.getStats();

        if (!CHAT_MODE) {
          std::cout << stats << std::endl;
        }
      }
      // tl.addData(user_input_span);
      // tl.encode(PRE_POST_PADDING);
      // tl.playAudioBlocking();

      // Restart the user input thread
      user_input_thread = std::thread(getUserInput);
    }

    if (rx_mode) {
      auto frameOpt = tl.getReceivedFrame();
      if (frameOpt.has_value()) {
        auto frame = frameOpt.value();
        auto data = frame.getData();
        std::string received_message(data.begin(), data.end());
        std::cout << "<RX>: " << received_message << std::endl;
      }

      auto stats = tl.getStats();
      if (!stats.is_quiet && !CHAT_MODE) {
        std::cout << stats << std::endl;
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  if (user_input_thread.joinable()) {
    user_input_thread.join();
  }

  if (rx_mode) {
    tl.stopListening();
  }

  return 0;
}