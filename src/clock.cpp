#include "clock.hpp"

RTClock<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClock =
    RTClock<RTC_IO, RTC_SCLK, RTC_CE>();