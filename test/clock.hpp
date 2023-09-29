#pragma once

#include <Arduino.h>
#include <RtcDS1302.h>

#include <chrono>
#include <cstdint>
#include <utility>

#include "RtcDateTime.h"
#include "SlowSoftI2CMaster.h"
#include "config.hpp"

template <uint8_t IO, uint8_t CLK, uint8_t CE>
class RTClock {
 private:
  ThreeWire m_rtc_wire{IO, CLK, CE};
  RtcDS1302<ThreeWire> m_rtc;

 public:
  RTClock() : m_rtc(m_rtc_wire) {
    m_rtc.Begin();
    if (!m_rtc.IsDateTimeValid()) {
      m_rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
    }
    m_rtc.SetIsWriteProtected(false);
    m_rtc.SetIsRunning(true);
  }
  ~RTClock() = default;

  RTClock(const RTClock&) = delete;
  RTClock& operator=(const RTClock&) = delete;
  RTClock(RTClock&&) noexcept = default;
  RTClock& operator=(RTClock&&) noexcept = default;

  inline RtcDateTime now() { return m_rtc.GetDateTime(); }
  inline time_t now_time_t() { return m_rtc.GetDateTime().Unix32Time(); }
  inline void set_time(uint32_t tv_sec) {
    m_rtc.SetDateTime(RtcDateTime(tv_sec - 946684800));
  }
};

template <uint8_t IO, uint8_t CLK, uint8_t CE>
class RTCUpdater {
 private:
  RTClock<IO, CLK, CE>& m_clock;
  timeval m_tm_buffer{0, 0};

 public:
  explicit RTCUpdater(RTClock<IO, CLK, CE>& clock)
      : m_clock(clock), m_tm_buffer{m_clock.now_time_t(), 0} {
    settimeofday(&m_tm_buffer, nullptr);
  }

  inline void set_time(uint32_t time) {
    m_tm_buffer.tv_sec = time;
    settimeofday(&m_tm_buffer, nullptr);
  }

  inline RtcDateTime last_time() {
    gettimeofday(&m_tm_buffer, nullptr);

    // -30 year
    return RtcDateTime(m_tm_buffer.tv_sec - 946684800);
  }
};

void rtclock_visualizer_task(void* arg);

template <uint8_t SDA, uint8_t SCL>
class RTClockVisualizer {
 private:
  SlowSoftI2CMaster si{SDA, SCL};

  uint8_t buf_pos_1{0};
  uint8_t buf_pos_2{0};
  uint8_t buf_pos_3{0};
  uint8_t buf_pos_4{0};

  bool m_dot{false};
  TaskHandle_t m_task_handle{nullptr};

 public:
  RTClockVisualizer() {
    si.i2c_init();
    si.i2c_start_wait(LED_VISUALIZE_DEV_ADDR);
    si.i2c_write(0b01110001);
  }

  void visualize(uint8_t n1, uint8_t n2, uint8_t n3, uint8_t n4, bool dot) {
    uint8_t dot_mask = dot ? LED_VISUALIZE_DOT_MASK : 0x00;
    uint8_t dot_mask_inversed = dot ? LED_VISUALIZE_DOT_MASK_INVERSED : 0x00;

    si.i2c_rep_start(LED_VISUALIZE_ADDR_TABLE[0]);
    si.i2c_write(LED_VISUALIZE_NUM_TABLE[n1]);

    si.i2c_rep_start(LED_VISUALIZE_ADDR_TABLE[1]);
    si.i2c_write(LED_VISUALIZE_NUM_TABLE[n2] | dot_mask);

    si.i2c_rep_start(LED_VISUALIZE_ADDR_TABLE[2]);
    si.i2c_write(LED_VISUALIZE_NUM_TABLE_INVERSED[n3] | dot_mask_inversed);

    si.i2c_rep_start(LED_VISUALIZE_ADDR_TABLE[3]);
    si.i2c_write(LED_VISUALIZE_NUM_TABLE[n4]);
  }

  inline void set_visualize_year(uint16_t year) {
    buf_pos_1 = year / 1000;
    buf_pos_2 = (year % 1000) / 100;
    buf_pos_3 = (year % 100) / 10;
    buf_pos_4 = year % 10;
    // buffer.first = year / 100;
    // buffer.second = year % 100;
    m_dot = false;
  }

  inline void set_visualize_month_day(uint8_t month, uint8_t day) {
    // buffer.first = month;
    // buffer.second = day;
    buf_pos_1 = month / 10;
    buf_pos_2 = month % 10;
    buf_pos_3 = day / 10;
    buf_pos_4 = day % 10;
    m_dot = false;
  }

  inline void set_visualize_hour_minute(uint8_t hour, uint8_t minute) {
    // buffer.first = hour;
    // buffer.second = minute;
    buf_pos_1 = hour / 10;
    buf_pos_2 = hour % 10;
    buf_pos_3 = minute / 10;
    buf_pos_4 = minute % 10;
    m_dot = true;
  }

  inline void set_visualize_minute_second(uint8_t minute, uint8_t second) {
    buf_pos_1 = minute / 10;
    buf_pos_2 = minute % 10;
    buf_pos_3 = second / 10;
    buf_pos_4 = second % 10;
    m_dot = true;
  }

  inline void visualize_pair_of_number(uint8_t n1, uint8_t n2, bool dot) {
    visualize(n1 / 10, n1 % 10, n2 / 10, n2 % 10, dot);
  }

  inline void visualize_self() {
    visualize(buf_pos_1, buf_pos_2, buf_pos_3, buf_pos_4, m_dot);
    // visualize(buffer.first / 10, buffer.first % 10, buffer.second / 10,
    //           buffer.second % 10, m_dot);
  }

  inline void set_pair_of_number(uint8_t n1, uint8_t n2) {
    buf_pos_1 = n1 / 10;
    buf_pos_2 = n1 % 10;
    buf_pos_3 = n2 / 10;
    buf_pos_4 = n2 % 10;
  }

  inline void set_buf(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4) {
    buf_pos_1 = b1;
    buf_pos_2 = b2;
    buf_pos_3 = b3;
    buf_pos_4 = b4;
  }

  inline void set_dot(bool dot) { m_dot = dot; }

  inline void begin() {
    xTaskCreate(rtclock_visualizer_task, "visualizer", 1024, nullptr, 1,
                &m_task_handle);
  }
};

extern RTClock<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClock;
extern RTCUpdater<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClockUpdater;
extern RTClockVisualizer<LED_VISUALIZE_USING_SDA, LED_VISUALIZE_USING_SCL>
    GlobalRTClockVisualizer;
