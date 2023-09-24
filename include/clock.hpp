#pragma once

#include <Arduino.h>
#include <RtcDS1302.h>

#include <cstdint>

#include "utils.hpp"

template <uint8_t IO, uint8_t CLK, uint8_t CE>
class RTClock {
 private:
  ThreeWire m_rtc_wire{IO, CLK, CE};
  RtcDS1302<ThreeWire> m_rtc{m_rtc_wire};

 public:
  RTClock();
  ~RTClock() = default;

  RTClock(const RTClock&) = delete;
  RTClock& operator=(const RTClock&) = delete;
  RTClock(RTClock&&) noexcept = default;
  RTClock& operator=(RTClock&&) noexcept = default;

  RtcDateTime now();
};

using GlobalRTClock = Singleton<RTClock<3, 2, 5>>;
