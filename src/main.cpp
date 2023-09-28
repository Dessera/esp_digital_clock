#include <Arduino.h>

#include "clock.hpp"

void setup() {
  Serial.begin(LOGGER_USING_BAUD);
}



void loop() {
  static RtcDateTime last_time{};
  static RtcDateTime now{};
  now = GlobalRTClock.now();
  if (now != last_time) {

    last_time = now;
  }
}
