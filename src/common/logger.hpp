/// @author Joshua Jerred
/// @copyright Copyright (c) 2025
/// @file logger.hpp
/// @date 2025-03-14

#pragma once

#include <iostream>
#include <string>

namespace kazoo {

class Logger {
 public:
  Logger(const std::string &logger_name)
      : logger_name_(logger_name) {
  }

 protected:
  void logWarning(const std::string &message) {
    log("WARNING", message);
  }

  void logError(const std::string &message) {
    log("WARNING", message);
  }

  void logInfo(const std::string &message) {
    log("INFO", message);
  }

  void logDebug(const std::string &message) {
    log("DEBUG", message);
  }

 private:
  void log(const std::string &level, const std::string &message) {
    std::cout << "[" << level << "] " << logger_name_ << ": " << message
              << std::endl;
  }

  std::string logger_name_;
};

}  // namespace kazoo