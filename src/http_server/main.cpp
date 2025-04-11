/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file main.cpp
/// @date 2025-04-11

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "tcp_socket.hpp"

inline constexpr int SERVER_PORT = 8082;

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

int main() {
  sock::TcpSocketServer server;
  if (!server.init(SERVER_PORT)) {
    std::cout << "Failed to initialize server" << std::endl;
    return -1;
  }

  sock::TcpSocketServer client;

  while (true) {
    if (!server.accept(client)) {
      // std::cout << "Failed to accept client" << std::endl;
      // return -1;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }

    std::cout << "Client connected" << std::endl;
    HttpResponse response;
    response.body = "Hello, World!";
    if (!client.send(response.getResponse())) {
      std::cout << "Failed to send message" << std::endl;
      return -1;
    }
  }
  return 0;
}