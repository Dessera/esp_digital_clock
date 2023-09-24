#include "logger.hpp"

void Logger::log(LogLevel level, const char* msg) {
  this->_print_log_level(level);
  m_serial.println(msg);
}

void Logger::format(LogLevel level, const char* fmt, ...) {
  this->_print_log_level(level);
  va_list args;
  va_start(args, fmt);
  m_serial.printf(fmt, args);
  va_end(args);
  m_serial.println();
}

Logger SerialLogger = Logger(Serial);
