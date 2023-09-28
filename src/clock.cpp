#include "clock.hpp"

RTClock<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClock =
    RTClock<RTC_IO, RTC_SCLK, RTC_CE>();

RTCUpdater<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClockUpdater =
    RTCUpdater<RTC_IO, RTC_SCLK, RTC_CE>(GlobalRTClock);

RTClockVisualizer<LED_VISUALIZE_USING_SDA, LED_VISUALIZE_USING_SCL>
    GlobalRTClockVisualizer =
        RTClockVisualizer<LED_VISUALIZE_USING_SDA, LED_VISUALIZE_USING_SCL>();
