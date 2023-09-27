#include <Arduino.h>
#include "clock.hpp"


void setup() {
  Serial.begin(LOGGER_USING_BAUD);
  GlobalRTClockUpdater.begin(RTC_UPDATE_USING_INTERVAL,
                             []() { GlobalRTClockUpdater.update(); });
}

void loop() {
  static RtcDateTime stored_time{0};
  if (GlobalRTClockUpdater.last_time() != stored_time) {
    stored_time = GlobalRTClockUpdater.last_time();
    Serial.printf("%02d:%02d:%02d\n", stored_time.Hour(), stored_time.Minute(),
                  stored_time.Second());
  }
}
