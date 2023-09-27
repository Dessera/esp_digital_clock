#include "clock.hpp"

RTClock<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClock =
    RTClock<RTC_IO, RTC_SCLK, RTC_CE>();

extern RTCUpdater<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClockUpdater =
    RTCUpdater<RTC_IO, RTC_SCLK, RTC_CE>(GlobalRTClock, RTC_UPDATE_USING_TIMER,
                                         RTC_UPDATE_USING_DIVIDER);
