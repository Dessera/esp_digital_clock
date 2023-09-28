#pragma once

#include <Arduino.h>
#include <RtcDS1302.h>
#include <sys/_timeval.h>
#include <sys/select.h>

#include <chrono>
#include <cstdint>

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

  inline RtcDateTime last_time() {
    gettimeofday(&m_tm_buffer, nullptr);
    return RtcDateTime(m_tm_buffer.tv_sec);
  }
};

template <uint8_t SDA, uint8_t SCL>
class RTClockVisualizer {
 private:
  SlowSoftI2CMaster si{SDA, SCL};

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
    si.i2c_write(LED_VISUALIZE_NUM_TABLE[n1 / 10]);

    si.i2c_rep_start(LED_VISUALIZE_ADDR_TABLE[1]);
    si.i2c_write(LED_VISUALIZE_NUM_TABLE[n2 % 10] | dot_mask);

    si.i2c_rep_start(LED_VISUALIZE_ADDR_TABLE[2]);
    si.i2c_write(LED_VISUALIZE_NUM_TABLE_INVERSED[n3 / 10] | dot_mask_inversed);

    si.i2c_rep_start(LED_VISUALIZE_ADDR_TABLE[3]);
    si.i2c_write(LED_VISUALIZE_NUM_TABLE[n4 % 10]);
  }

  void visualize_year(uint16_t year) {
    visualize(year / 1000, (year / 100) % 10, (year / 10) % 10, year % 10,
              false);
  }

  void visualize_month_day(uint8_t month, uint8_t day) {
    visualize(month / 10, month % 10, day / 10, day % 10, true);
  }

  void visualize_hour_minute(uint8_t hour, uint8_t minute) {
    visualize(hour / 10, hour % 10, minute / 10, minute % 10, true);
  }

  void visualize_minute_second(uint8_t second) {
    visualize(0, 0, second / 10, second % 10, true);
  }
};

extern RTClock<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClock;
extern RTCUpdater<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClockUpdater;
extern RTClockVisualizer<LED_VISUALIZE_USING_SDA, LED_VISUALIZE_USING_SCL>
    GlobalRTClockVisualizer;
