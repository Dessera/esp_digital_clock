#pragma once

#include <Arduino.h>
#include <RtcDS1302.h>

#include <cstdint>

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

extern RTClock<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClock;
