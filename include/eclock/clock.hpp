#pragma once

#include <cstdint>

#include "RtcDS1302.h"
#include "SlowSoftI2CMaster.h"
#include "ThreeWire.h"
#include "eclock/config.hpp"

/**
 * @brief DS1302 RTC Wrapper
 *
 * @tparam IO Pin number of IO
 * @tparam CLK Pin number of CLK
 * @tparam CE Pin number of CE / RST
 */
template <uint8_t IO, uint8_t CLK, uint8_t CE>
class RTClock {
 public:
  /**
   * @brief offset of time_t and RTCDatetime
   *
   */
  constexpr static uint32_t RTC_TM_OFFSET = 946684800;

 private:
  ThreeWire m_rtc_wire{IO, CLK, CE};
  RtcDS1302<ThreeWire> m_rtc{m_rtc_wire};
  timeval m_tm_buffer{0, 0};

 public:
  RTClock() {
    m_rtc.Begin();
    if (!m_rtc.IsDateTimeValid()) {
      m_rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
    }
    if (m_rtc.GetIsWriteProtected()) {
      m_rtc.SetIsWriteProtected(false);
    }
    if (!m_rtc.GetIsRunning()) {
      m_rtc.SetIsRunning(true);
    }

    m_tm_buffer.tv_sec = m_rtc.GetDateTime().Unix32Time();
    settimeofday(&m_tm_buffer, nullptr);
  }

  ~RTClock() = default;

  RTClock(const RTClock&) = delete;
  RTClock& operator=(const RTClock&) = delete;
  RTClock(RTClock&&) noexcept = default;
  RTClock& operator=(RTClock&&) noexcept = default;

  inline void set_time(time_t tv_sec) {
    m_tm_buffer.tv_sec = tv_sec;
    settimeofday(&m_tm_buffer, nullptr);
    m_rtc.SetDateTime(RtcDateTime(tv_sec - RTC_TM_OFFSET));
  }

  inline time_t get_time() {
    gettimeofday(&m_tm_buffer, nullptr);
    return m_tm_buffer.tv_sec;
  }
};

/**
 * @brief Visualizer for DS1302 RTC
 *
 * @tparam SDA SDA Pin number
 * @tparam SCL SCL Pin number
 * @tparam DEV I2C Device address
 */
template <uint8_t SDA, uint8_t SCL, uint8_t DEV>
class RTClockVisualizer {
 public:
  using ScreenBuffer = uint8_t[4];
  using ScreenConfig = uint8_t;

  constexpr static uint8_t LED_VISUALIZE_NUM_TABLE[10] = {
      0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
  constexpr static uint8_t LED_VISUALIZE_NUM_TABLE_INVERSED[10] = {
      0xaf, 0xa0, 0xC7, 0xE5, 0xE8, 0x6d, 0x6F, 0xA4, 0xEF, 0xED};
  constexpr static uint8_t LED_POS_TABLE[4] = {0x68, 0x6A, 0x6C, 0x6E};
  constexpr static uint8_t LED_VISUALIZE_DOT = 0x80;
  constexpr static uint8_t LED_VISUALIZE_DOT_INVERSED = 0x10;

  constexpr static uint8_t BRIGHTNESS_MASK = 0x80;
  constexpr static uint8_t DISPLAY_MASK = 0x01;

 private:
  SlowSoftI2CMaster prot{SDA, SCL};
  ScreenBuffer m_buffer{0, 0, 0, 0};
  ScreenConfig m_config{0};

 public:
  explicit RTClockVisualizer(uint8_t brightness = 0b00000111,
                             bool display = true)
      : m_config(brightness << 4 | (display ? 0x01 : 0x00)) {
    prot.i2c_init();
    prot.i2c_start_wait(DEV);
    prot.i2c_write(m_config);
  }

  ~RTClockVisualizer() { prot.i2c_stop(); }

  /**
   * @brief Set the brightness of LED
   *
   * @param brightness Brightness of LED (0x00 ~ 0x07)
   */
  void set_brightness(uint8_t brightness) {
    m_config =
        ((brightness << 4) & BRIGHTNESS_MASK) | (m_config & ~BRIGHTNESS_MASK);
  }

  /**
   * @brief Set the display of LED
   *
   * @param display Display LED or not
   */
  void set_display(bool display) {
    m_config = (display ? 0x01 : 0x00) | (m_config & ~DISPLAY_MASK);
  }

  /**
   * @brief Send config to LED
   *
   */
  void sync_config() {
    prot.i2c_rep_start(DEV);
    prot.i2c_write(m_config);
  }

  /**
   * @brief Send buffer to LED
   *
   */
  void sync_buffer() {
    prot.i2c_rep_start(LED_POS_TABLE[0]);
    prot.i2c_write(m_buffer[0]);

    prot.i2c_rep_start(LED_POS_TABLE[1]);
    prot.i2c_write(m_buffer[1]);

    prot.i2c_rep_start(LED_POS_TABLE[2]);
    prot.i2c_write(m_buffer[2]);

    prot.i2c_rep_start(LED_POS_TABLE[3]);
    prot.i2c_write(m_buffer[3]);
  }

  /**
   * @brief Set the num of specific position
   *
   * @param pos num position (0 ~ 3)
   * @param num num (0 ~ 9)
   */
  void set_num(uint8_t pos, uint8_t num) {
    if (pos > 3 || num > 9) {
      return;
    }

    uint8_t num_mask =
        pos == 2 ? LED_VISUALIZE_DOT_INVERSED : LED_VISUALIZE_DOT;
    uint8_t num_map = pos == 2 ? LED_VISUALIZE_NUM_TABLE_INVERSED[num]
                               : LED_VISUALIZE_NUM_TABLE[num];
    m_buffer[pos] &= num_mask;
    m_buffer[pos] |= num_map;
  }

  /**
   * @brief Set the dot of specific position
   *
   * @param pos dot position (0 ~ 3)
   * @param dot dot or not
   */
  void set_dot(uint8_t pos, bool dot) {
    if (pos > 3) {
      return;
    }

    uint8_t dot_mask =
        pos == 2 ? LED_VISUALIZE_DOT_INVERSED : LED_VISUALIZE_DOT;
    m_buffer[pos] = (m_buffer[pos] & ~dot_mask) | (dot ? dot_mask : 0x00);
  }

  /**
   * @brief Set 2-digit num to specific position
   *
   * @param pos num position (0 ~ 2)
   * @param num num (0 ~ 99)
   */
  inline void set_wnum(uint8_t pos, uint8_t num) {
    set_num(pos, num / 10);
    set_num(pos + 1, num % 10);
  }

  /**
   * @brief Set the left screen num (2-digit)
   *
   * @param num num (0 ~ 99)
   */
  inline void set_lnum(uint8_t num) { set_wnum(0, num); }

  /**
   * @brief Set the right screen num (2-digit)
   *
   * @param num num (0 ~ 99)
   */
  inline void set_rnum(uint8_t num) { set_wnum(2, num); }

  /**
   * @brief Set the fullnum (4-digit)
   *
   * @param num num (0 ~ 9999)
   */
  inline void set_fullnum(uint16_t num) {
    set_lnum(num / 100);
    set_rnum(num % 100);
  }

  /**
   * @brief Set the ":" symbol
   *
   * @param dot dot or not
   */
  inline void set_wdot(bool dot) {
    set_dot(1, dot);
    set_dot(2, dot);
  }

  /**
   * @brief disable the display of specific position
   *
   * @param pos num position (0 ~ 3)
   */
  void unset_num(uint8_t pos) {
    if (pos > 3) {
      return;
    }

    m_buffer[pos] &=
        (pos == 2 ? LED_VISUALIZE_DOT_INVERSED : LED_VISUALIZE_DOT);
  }

  /**
   * @brief disable the display of 2-digit num
   *
   * @param pos num position (0 ~ 2)
   */
  inline void unset_wnum(uint8_t pos) {
    unset_num(pos);
    unset_num(pos + 1);
  }

  /**
   * @brief disable the display of left screen num (2-digit)
   *
   */
  inline void unset_lnum() { unset_wnum(0); }

  /**
   * @brief disable the display of right screen num (2-digit)
   *
   */
  inline void unset_rnum() { unset_wnum(2); }
};

extern RTClock<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClock;
extern RTClockVisualizer<SCREEN_SDA, SCREEN_SCL, SCREEN_ADDR>
    GlobalRTClockVisualizer;
