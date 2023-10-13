#pragma once

#include <sys/_timeval.h>

#include <cstddef>
#include <cstdint>
#include <queue>
#include <vector>

#include "Preferences.h"
#include "RtcDS1302.h"
#include "SlowSoftI2CMaster.h"
#include "ThreeWire.h"
#include "eclock/config.hpp"
#include "eclock/music.hpp"
#include "esp32-hal-gpio.h"
#include "esp32-hal-ledc.h"

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

  inline void get_tm_struct(tm* tm_struct) {
    time_t tm = get_time();
    localtime_r(&tm, tm_struct);
    fmt_time_struct(tm_struct);
  }

  inline void set_tm_struct(tm* tm_struct) {
    defmt_time_struct(tm_struct);
    set_time(mktime(tm_struct));
  }

  inline void fmt_time_struct(tm* tm_struct) {
    tm_struct->tm_year += 1900;
    tm_struct->tm_mon += 1;
    tm_struct->tm_wday += 1;
  }

  inline void defmt_time_struct(tm* tm_struct) {
    tm_struct->tm_year -= 1900;
    tm_struct->tm_mon -= 1;
    tm_struct->tm_wday -= 1;
  }
};

constexpr static uint8_t LED_VISUALIZE_NUM_TABLE[11] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x00};
constexpr static uint8_t LED_POS_TABLE[4] = {0x68, 0x6A, 0x6C, 0x6E};

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

  constexpr static uint8_t LED_VISUALIZE_DOT = 0x80;

  constexpr static uint8_t BRIGHTNESS_MASK = 0x07;
  constexpr static uint8_t DISPLAY_MASK = 0x01;

 private:
  SlowSoftI2CMaster prot{SDA, SCL};
  ScreenBuffer m_buffer{0, 0, 0, 0};
  ScreenConfig m_config{0};

  bool buffer_edited_flag{false};
  bool config_edited_flag{false};

 public:
  explicit RTClockVisualizer(uint8_t brightness = 0x07)
      : m_config(brightness << 4 | 0x01) {
    prot.i2c_init();
    prot.i2c_start_wait(DEV);
    prot.i2c_write(m_config);
  }

  ~RTClockVisualizer() { prot.i2c_stop(); }

  inline bool is_buffer_edited() { return buffer_edited_flag; }
  inline void clear_buffer_edited_flag() { buffer_edited_flag = false; }
  inline bool is_config_edited() { return config_edited_flag; }
  inline void clear_config_edited_flag() { config_edited_flag = false; }

  /**
   * @brief Set the brightness of LED
   *
   * @param brightness Brightness of LED (0x00 ~ 0x07)
   */
  void set_brightness(uint8_t brightness) {
    m_config = (brightness << 4) | 0x01;
    Preferences pref;
    pref.begin("eclock", false);
    pref.putChar("brightness", get_brightness());
    pref.end();
    config_edited_flag = true;
  }

  uint8_t get_brightness() { return m_config >> 4; }

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
   * @param num num (-1 represents none)
   */
  void set_num(uint8_t pos, int8_t num) {
    if (pos > 3 || num > 9) {
      return;
    }
    num = num < 0 ? 10 : num;

    m_buffer[pos] &= LED_VISUALIZE_DOT;
    m_buffer[pos] |= LED_VISUALIZE_NUM_TABLE[num];

    buffer_edited_flag = true;
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

    m_buffer[pos] =
        (m_buffer[pos] & ~LED_VISUALIZE_DOT) | (dot ? LED_VISUALIZE_DOT : 0x00);

    buffer_edited_flag = true;
  }

  /**
   * @brief Set 2-digit num to specific position
   *
   * @param pos num position (0 ~ 2)
   * @param num num (-1 represents none)
   */
  inline void set_wnum(uint8_t pos, int8_t num) {
    if (num < 0) {
      set_num(pos, -1);
      set_num(pos + 1, -1);
    } else {
      set_num(pos, num / 10);
      set_num(pos + 1, num % 10);
    }
  }

  /**
   * @brief Set the left screen num (2-digit)
   *
   * @param num num (-1 represents none)
   */
  inline void set_lnum(int8_t num) { set_wnum(0, num); }

  /**
   * @brief Set the right screen num (2-digit)
   *
   * @param num num (-1 represents none)
   */
  inline void set_rnum(int8_t num) { set_wnum(2, num); }

  /**
   * @brief Set the fullnum (4-digit)
   *
   * @param num num (-1 represents none)
   */
  inline void set_fullnum(int16_t num) {
    if (num < 0) {
      set_lnum(-1);
      set_rnum(-1);
    } else {
      set_lnum(num / 100);
      set_rnum(num % 100);
    }
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
};

template <uint8_t BEEP>
class RTClockAlarmWatcher {
 private:
  uint8_t beep_strength;

  time_t start{0};
  time_t end{0};

 public:
  explicit RTClockAlarmWatcher(uint8_t strength) : beep_strength(strength) {
    ledcSetup(0, 1000, 8);
    ledcAttachPin(BEEP, 0);
  }
  ~RTClockAlarmWatcher() = default;

  RTClockAlarmWatcher(const RTClockAlarmWatcher&) = delete;
  RTClockAlarmWatcher& operator=(const RTClockAlarmWatcher&) = delete;
  RTClockAlarmWatcher(RTClockAlarmWatcher&&) noexcept = default;
  RTClockAlarmWatcher& operator=(RTClockAlarmWatcher&&) noexcept = default;

  inline int get_beep_strength() { return beep_strength; }
  inline void set_beep_strength(uint8_t strength) {
    beep_strength = strength;
    Preferences pref;
    pref.begin("eclock", false);
    pref.putLong("beep_strength", strength);
    pref.end();
  }

  inline void enable_alarm(time_t alarm) {
    // ensure alarm is sorted, so use binary search to insert
    start = alarm;
    end = alarm + 271;
    Preferences pref;
    pref.begin("eclock", false);
    pref.putLong("alarm_time", alarm);
    pref.end();
  }

  inline bool is_alarm_enabled() { return start != 0 && end != 0; }

  inline void disable_alarm() {
    start = 0;
    end = 0;
    Preferences pref;
    pref.begin("eclock", false);
    pref.putLong("alarm_time", 0);
    pref.end();
  }

  inline void move_to_next_day() {
    if (start == 0 && end == 0) {
      return;
    }
    start += 86400;
    end += 86400;
    Preferences pref;
    pref.begin("eclock", false);
    pref.putLong("alarm_time", start);
    pref.end();
  }

  void spin_once(time_t current) {
    static uint8_t index = 0;
    if (start == 0 && end == 0) {
      index = 0;
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      return;
    }

    if (current > end) {
      // move to next day
      start += 86400;
      end += 86400;
      index = 0;
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      return;
    }

    if (current < start) {
      index = 0;
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      return;
    }

    int music_index = music[index];
    index = (index + 1) % 271;

    if (music_index == 0) {
      vTaskDelay(166 / portTICK_PERIOD_MS);
    } else {
      ledcWrite(0, beep_strength);
      ledcChangeFrequency(0, select_music(music_index), 10);
      vTaskDelay(166 / portTICK_PERIOD_MS);
      ledcWrite(0, 0);
    }
  }
};

extern RTClock<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClock;
extern RTClockVisualizer<SCREEN_SDA, SCREEN_SCL, SCREEN_ADDR>
    GlobalRTClockVisualizer;
extern RTClockAlarmWatcher<BEEP_PIN> GlobalRTClockAlarmWatcher;
