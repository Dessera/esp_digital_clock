#pragma once

#include <array>
#include <cstdint>

constexpr uint8_t RTC_SCLK = 19;
constexpr uint8_t RTC_IO = 3;
constexpr uint8_t RTC_CE = 2;

constexpr uint8_t LED_VISUALIZE_USING_SDA = 4;
constexpr uint8_t LED_VISUALIZE_USING_SCL = 5;

constexpr uint8_t LED_VISUALIZE_DEV_ADDR = 0x48;
constexpr uint8_t LED_VISUALIZE_DOT_MASK = 0x80;
constexpr uint8_t LED_VISUALIZE_DOT_MASK_INVERSED = 0x10;

constexpr std::array<uint8_t, 4> LED_VISUALIZE_ADDR_TABLE{0x68, 0x6A, 0x6C,
                                                          0x6E};
constexpr std::array<uint8_t, 11> LED_VISUALIZE_NUM_TABLE{
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x00};
constexpr std::array<uint8_t, 11> LED_VISUALIZE_NUM_TABLE_INVERSED = {
    0xaf, 0xa0, 0xC7, 0xE5, 0xE8, 0x6d, 0x6F, 0xA4, 0xEF, 0xED, 0x00};

constexpr uint8_t BUTTON_1_USING_PIN = 13;
constexpr uint8_t BUTTON_2_USING_PIN = 10;
constexpr uint8_t BUTTON_3_USING_PIN = 7;
constexpr uint8_t BUTTON_PRESS_THRESHOLD = 10;
constexpr uint8_t BUTTON_LONG_PRESS_THRESHOLD = 50;

constexpr uint32_t LOGGER_USING_BAUD = 9600;

// constexpr uint8_t
