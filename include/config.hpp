#pragma once

#include <cstdint>

constexpr uint8_t RTC_SCLK = 2;
constexpr uint8_t RTC_IO = 3;
constexpr uint8_t RTC_CE = 5;

constexpr uint8_t RTC_UPDATE_USING_TIMER = 0;
constexpr uint8_t RTC_UPDATE_USING_DIVIDER = 80;
constexpr uint8_t RTC_UPDATE_USING_INTERVAL = 100000;

constexpr uint32_t LOGGER_USING_BAUD = 9600;

// constexpr uint8_t 
