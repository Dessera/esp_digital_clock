#pragma once

#include "HardwareSerial.h"

enum class LogLevel : uint8_t { DEBUG, INFO, WARN, ERROR };

class Logger {
 private:
  HardwareSerial& m_serial;

 public:
  explicit Logger(HardwareSerial& serial) : m_serial(serial){};
  ~Logger() = default;

  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(Logger&&) noexcept = default;
  Logger& operator=(Logger&&) noexcept = delete;

  void log(LogLevel level, const char* msg);
  void format(LogLevel level, const char* fmt, ...);

  inline void info(const char* msg) { log(LogLevel::INFO, msg); }
  inline void debug(const char* msg) { log(LogLevel::DEBUG, msg); }
  inline void warn(const char* msg) { log(LogLevel::WARN, msg); }
  inline void error(const char* msg) { log(LogLevel::ERROR, msg); }

  inline void fmt_info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    format(LogLevel::INFO, fmt, args);
    va_end(args);
  }

  inline void fmt_debug(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    format(LogLevel::DEBUG, fmt, args);
    va_end(args);
  }

  inline void fmt_warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    format(LogLevel::WARN, fmt, args);
    va_end(args);
  }

  inline void fmt_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    format(LogLevel::ERROR, fmt, args);
    va_end(args);
  }

 private:
  inline void _print_log_level(LogLevel level) {
    switch (level) {
      case LogLevel::DEBUG:
        m_serial.print("[DEBUG] ");
        break;
      case LogLevel::INFO:
        m_serial.print("[INFO] ");
        break;
      case LogLevel::WARN:
        m_serial.print("[WARN] ");
        break;
      case LogLevel::ERROR:
        m_serial.print("[ERROR] ");
        break;
    }
  }
};

extern Logger SerialLogger;
