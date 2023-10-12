#pragma once

#include <cstdint>

#pragma region Configurations

constexpr uint8_t RTC_SCLK = 19;
constexpr uint8_t RTC_IO = 3;
constexpr uint8_t RTC_CE = 2;

constexpr uint8_t SCREEN_SDA = 4;
constexpr uint8_t SCREEN_SCL = 5;
constexpr uint8_t SCREEN_ADDR = 0x48;

constexpr uint8_t BTN_1_PIN = 10;
constexpr uint8_t BTN_2_PIN = 13;
constexpr uint8_t BTN_3_PIN = 7;
constexpr uint8_t BTN_PRESS_THRESHOLD = 10;
constexpr uint8_t BTN_LONG_PRESS_THRESHOLD = 50;

constexpr uint8_t BEEP_PIN = 18;
constexpr uint8_t BEEP_STRENGTH = 50;

constexpr uint32_t LOGGER_BAUD = 9600;

#pragma endregion

#pragma region Utils

#define ECLOCK_NS_START namespace eclock {
#define ECLOCK_NS_END }

#pragma endregion