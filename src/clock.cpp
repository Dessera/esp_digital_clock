#include "eclock/clock.hpp"

RTClock<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClock{};
RTClockVisualizer<SCREEN_SDA, SCREEN_SCL, SCREEN_ADDR>
    GlobalRTClockVisualizer{};

// RTClock<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClock =
//     RTClock<RTC_IO, RTC_SCLK, RTC_CE>();

// RTCUpdater<RTC_IO, RTC_SCLK, RTC_CE> GlobalRTClockUpdater =
//     RTCUpdater<RTC_IO, RTC_SCLK, RTC_CE>(GlobalRTClock);

// RTClockVisualizer<LED_VISUALIZE_USING_SDA, LED_VISUALIZE_USING_SCL>
//     GlobalRTClockVisualizer =
//         RTClockVisualizer<LED_VISUALIZE_USING_SDA,
//         LED_VISUALIZE_USING_SCL>();

// void rtclock_visualizer_task(void* arg) {

//   while (true) {
//     GlobalRTClockVisualizer.visualize_self();
//     vTaskDelay(1000 / portTICK_PERIOD_MS);
//   }
// }
