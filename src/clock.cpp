#include "clock.hpp"

void global_rtc_updater_fn() { GlobalRTClockUpdater.update(); }

RTClock<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClock =
    RTClock<RTC_IO, RTC_SCLK, RTC_CE>();

RTCUpdater<RTC_IO, RTC_SCLK, RTC_CE>
    GlobalRTClockUpdater =
        RTCUpdater<RTC_IO, RTC_SCLK, RTC_CE>(
            GlobalRTClock, RTC_UPDATE_USING_TIMER, RTC_UPDATE_USING_DIVIDER,
            RTC_UPDATE_USING_INTERVAL, global_rtc_updater_fn);