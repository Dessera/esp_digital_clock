#pragma once

#include <Arduino.h>
#include <RtcDS1302.h>

#include <cstdint>

#include "RtcDateTime.h"
#include "config.hpp"
#include "logger.hpp"

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
};

template <uint8_t IO, uint8_t CLK, uint8_t CE>
class RTCUpdater {
 private:
  hw_timer_t* m_timer;
  RtcDateTime m_last_time;
  RTClock<IO, CLK, CE>& m_clock;

 public:
  RTCUpdater(RTClock<IO, CLK, CE>& clock, uint8_t timer_id, uint8_t divider,
             uint32_t interval, void (*callback)() = nullptr)
      : m_timer(timerBegin(timer_id, divider, true)), m_clock(clock) {
    timerAttachInterrupt(m_timer, callback, true);
    timerAlarmWrite(m_timer, interval, true);
    timerAlarmEnable(m_timer);
  }

  inline RtcDateTime& last_time() { return m_last_time; }
  inline void update() { m_last_time = m_clock.now(); }
};

class GlobalRTClockUpdaterFn;

extern RTClock<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClock;
extern RTCUpdater<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClockUpdater;
