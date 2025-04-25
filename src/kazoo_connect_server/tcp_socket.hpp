/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file tcp_socket.hpp
/// @date 2025-04-11
/// @details This is a needless stretch goal of the project, I ripped off my own
/// implementation from a previous project, which uses another reference.
/// https://github.com/joshua-jerred/Giraffe/blob/main/src/common/socket.hpp

#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

// #include "optional_logger.hpp"

// #include <BoosterSeat/rolling_average.hpp>

namespace sock {
constexpr int K_MAX_HOST_NAME_SIZE = 200;
constexpr int K_MAX_CONNECTIONS = 5;
constexpr int K_MAX_RECEIVE = 2000;

/**
 * @brief The TCP socket server class.
 * @ingroup ServerModule
 */
class TcpSocketServer {
 public:
  TcpSocketServer() {
    memset(&addr_, 0, sizeof(addr_));
  }
  ~TcpSocketServer() {
    if (isValid()) ::close(sock_);
  }

  bool init(int port_number) {
    if (!create()) {
      // std::cout << "Failed to create" << std::endl;
      return false;
    }

    // Non-Blocking
    constexpr bool non_blocking = true;
    int opts;
    opts = fcntl(sock_, F_GETFL);
    if (opts >= 0) {
      if (non_blocking)
        opts = (opts | O_NONBLOCK);
      else
        opts = (opts & ~O_NONBLOCK);
      fcntl(sock_, F_SETFL, opts);
    }

    if (!bind(port_number)) {
      // std::cout << "Failed to bind" << std::endl;
      return false;
    }

    if (!listen()) {
      // std::cout << "Failed to listen" << std::endl;
      return false;
    }

    return true;
  }

  bool create() {
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (!isValid()) return false;

    int on = 1;
    if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) ==
        -1) {
      return false;
    }

    return true;
  }

  bool bind(const int port) {
    if (!isValid()) {
      return false;
    }

    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = INADDR_ANY;
    addr_.sin_port = htons(port);

    int bind_status = ::bind(sock_, (struct sockaddr *)&addr_, sizeof(addr_));
    if (bind_status == -1) {
      return false;
    }
    return true;
  }

  bool listen() const {
    if (!isValid()) {
      return false;
    }

    int listen_status = ::listen(sock_, sock::K_MAX_CONNECTIONS);
    if (listen_status == -1) {
      return false;
    }
    return true;
  }

  bool accept(sock::TcpSocketServer &new_socket) {
    int address_length = sizeof(addr_);
    new_socket.sock_ = ::accept(sock_, (sockaddr *)&addr_, (socklen_t *)&address_length);

    if (new_socket.sock_ <= 0) {
      return false;
    }
    return true;
  }

  bool close() {
    if (isValid()) {
      ::close(sock_);
      sock_ = -1;
      return true;
    } else {
      return false;
    }
  }

  bool send(const std::string &data) const {
    int status = ::send(sock_, data.c_str(), data.size(), MSG_NOSIGNAL);
    if (status == -1) {
      return false;
    } else {
      return true;
    }
  }

  /// @todo spanify, no need to use pointers here
  bool sendRawData(const uint8_t *data, size_t size) {
    int status = ::send(sock_, data, size, 0);
    if (status == -1) {
      return false;
    } else {
      return true;
    }
  }

  bool receive(std::string &data) const {
    char buf[sock::K_MAX_RECEIVE + 1];

    data = "";

    memset(buf, 0, sock::K_MAX_RECEIVE + 1);

    int status = ::recv(sock_, buf, sock::K_MAX_RECEIVE, 0);

    if (status == -1) {
      return 0;
    } else if (status == 0) {
      return 0;
    } else {
      data = buf;
      return status;
    }
  }

  bool isValid() const {
    return sock_ != -1;
  }

  std::string getClientAddress() const {
    return inet_ntoa(addr_.sin_addr);
  }

 private:
  int sock_ = -1;
  sockaddr_in addr_ = {};
};

class TcpSocketClient
// : public giraffe::OptionalLogger
{
 public:
  TcpSocketClient() = default;

  // TcpSocketClient(giraffe::ILogger &logger) {
  // setLogger(logger);
  // }

  ~TcpSocketClient() = default;

  bool connect(std::string server_address_string, uint16_t port) {
    if (sock_ >= 0) {
      ::close(sock_);
      sock_ = -1;
    }

    sock_ = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_ < 0) {
      connected_ = false;
      // logError("connect: Failed to create socket");
      return false;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(server_address_string.c_str());

    if (::connect(sock_, (struct sockaddr *)&server_address, sizeof(server_address)) <
        0) {
      connected_ = false;
      // logError("connect: Failed to connect");
      return false;
    }

    // logDebug("connect: Success");
    connected_ = true;
    return true;
  }

  bool transaction(const std::string &data, std::string &response) {
    // logDebug("transaction: Start");
    if (!connected_) {
      // logError("transaction: Not connected");
      return false;
    }

    if (!send(data)) {
      // logError("transaction: Failed to send");
      return false;
    }

    if (!receive(response)) {
      // logError("transaction: Failed to receive");
      return false;
    }

    // logDebug("transaction: Success - sent: " + data + " received: " + response);
    return true;
  }

  bool disconnect() {
    if (sock_ >= 0) {
      ::close(sock_);
      sock_ = -1;
      connected_ = false;
      // logDebug("disconnect: Success");
      return true;
    } else {
      connected_ = false;
      return false;
    }
  }

 private:
  bool send(const std::string &data) {
    int status = ::send(sock_, data.c_str(), data.size(), MSG_NOSIGNAL);
    if (status == -1) {
      // logError("send: Failed to send");
      return false;
    } else {
      // logDebug("send: Success");
      return true;
    }
  }

  bool receive(std::string &data) {
    char buf[sock::K_MAX_RECEIVE + 1];

    data = "";

    memset(buf, 0, sock::K_MAX_RECEIVE + 1);

    int status = ::recv(sock_, buf, sock::K_MAX_RECEIVE, 0);

    if (status == -1) {
      return 0;
    } else if (status == 0) {
      return 0;
    } else {
      data = buf;
      return status;
    }

    return true;
  }

  int sock_ = -1;

  bool connected_ = false;
};

}  // namespace sock

#endif  // SOCKET_HPP_