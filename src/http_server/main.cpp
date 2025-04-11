/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file main.cpp
/// @date 2025-04-11

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "tcp_socket.hpp"

#include <ktl/translation_layer.hpp>

inline constexpr int SERVER_PORT = 8082;

static std::mutex s_messages_mutex;
static std::vector<std::string> s_messages_to_send = {};
static std::vector<std::string> s_messages_received = {};

kazoo::TranslationLayer s_kazooformation{
    kazoo::TranslationLayer::ModelType::K3_REASONABLE_MODEL};

struct HttpResponse {
  std::string body;

  std::string getResponse() const {
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/plain\r\n"
           "Content-Length: " +
           std::to_string(body.size()) +
           "\r\n"
           "\r\n" +
           body;
  }
};

void httpServerThread() {
  sock::TcpSocketServer server;
  if (!server.init(SERVER_PORT)) {
    std::cout << "Failed to initialize server" << std::endl;
    exit(-1);
    return;
  }

  sock::TcpSocketServer client;

  while (true) {
    if (!server.accept(client)) {
      // std::cout << "Failed to accept client" << std::endl;
      // return -1;

      // lazy...
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }

    std::cout << "Client connected" << std::endl;

    std::string request;
    if (!client.receive(request)) {
      std::cout << "Failed to receive message" << std::endl;
      continue;
    }

    std::string method;
    std::string path;
    std::string body;

    // parse it
    {
      size_t pos = request.find(" ");
      if (pos != std::string::npos) {
        method = request.substr(0, pos);
      }
      // std::cout << "-- METHOD: " << method << std::endl;

      pos = request.find(" ");
      if (pos != std::string::npos) {
        size_t pos2 = request.find(" ", pos + 1);
        if (pos2 != std::string::npos) {
          path = request.substr(pos + 1, pos2 - pos - 1);
        }
      }
      // std::cout << "-- PATH: " << path << std::endl;

      /// @todo confirm this behavior with spec
      // The json body comes after a double CRLF
      pos = request.find("\r\n\r\n");
      if (pos != std::string::npos) {
        body = request.substr(pos + 4);
      }
      // std::cout << "-- BODY: " << request << std::endl;
    }

    HttpResponse response;
    if (method == "GET") {
      std::cout << "GET Messages" << std::endl;
      response.body = "{\"messages\": [";
      std::lock_guard<std::mutex> lock(s_messages_mutex);

      for (size_t i = 0; i < s_messages_received.size(); ++i) {
        response.body += "{\"text\": \"" + s_messages_received[i] + "\"}";
        if (i != s_messages_received.size() - 1) {
          response.body += ",";
        }
      }
      response.body += "]";

      response.body += ", \"messages_to_send\": [";
      for (size_t i = 0; i < s_messages_to_send.size(); ++i) {
        response.body += "{\"text\": \"" + s_messages_to_send[i] + "\"}";
        if (i != s_messages_to_send.size() - 1) {
          response.body += ",";
        }
      }
      response.body += "]";

      response.body += "}";
    } else if (method == "POST") {
      std::cout << "POST Messages" << std::endl;
      std::cout << "Body: " << body << std::endl;
      std::lock_guard<std::mutex> lock(s_messages_mutex);
      s_messages_to_send.push_back(body);
      response.body = "{\"status\": \"ok\"}";
    } else {
      std::cout << "Unknown method: " << method << std::endl;
      continue;
    }

    if (!client.send(response.getResponse())) {
      std::cout << "Failed to send message" << std::endl;
      continue;
    }
  }
}

int main() {
  std::cout << "Starting HTTP server on port " << SERVER_PORT << std::endl;
  std::thread server_thread(httpServerThread);

  s_kazooformation.startListening();
  std::cout << "Kazooformation listening" << std::endl;
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // check if we have any messages to send
    s_messages_mutex.lock();
    if (!s_messages_to_send.empty()) {
      std::string message = s_messages_to_send.back();
      s_messages_to_send.pop_back();
      s_messages_mutex.unlock();  // unlock before sending, it's a blocking call

      std::cout << "Sending message: " << message << std::endl;
      kazoo::KtlFrame frame{message};

      // s_kazooformation.stopListening();   // we don't want our own audio
      s_kazooformation.sendFrame(frame);  // blocking send, this might take a bit
      // s_kazooformation.startListening();
    } else {
      s_messages_mutex.unlock();
    }

    // check if we have any messages received
    s_messages_mutex.lock();
    auto frameOpt = s_kazooformation.getReceivedFrame();
    if (frameOpt.has_value()) {
      auto frame = frameOpt.value();
      auto data = frame.getData();
      std::string received_message(data.begin(), data.end());
      std::cout << "<RX>: " << received_message << std::endl;
      s_messages_received.push_back(received_message);
    }
    s_messages_mutex.unlock();
  }

  /// @todo catch the ctrl+c signal and exit gracefully

  return 0;
}