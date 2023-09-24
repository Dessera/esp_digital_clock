#include <Arduino.h>
#include <sys/_stdint.h>

#include <array>
#include <unordered_map>

#include "ShiftRegister74HC595.h"
#include "clock.hpp"
#include "esp32-hal-timer.h"
#include "logger.hpp"
#include "utils.hpp"

// TODO: 将数码管操作封装
ShiftRegister74HC595<1> sr{8, 9, 2};
const std::array<uint8_t, 10> digits{
    0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
    0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111};

// TODO: 计时器同步操作封装
hw_timer_t* timer = nullptr;
RtcDateTime last_time{};
void timInterrupt() {
  last_time = GlobalRTClock::instance().now();
  SerialLogger::instance().fmt_info("Current time: %02d:%02d:%02d",
                                    last_time.Hour(), last_time.Minute(),
                                    last_time.Second());
}

void setup() {
  Serial.begin(9600);
  auto current = GlobalRTClock::instance().now();
  SerialLogger::instance().fmt_info("Current time: %02d:%02d:%02d",
                                    current.Hour(), current.Minute(),
                                    current.Second());

  timer = timerBegin(0, 0, true);
  timerAttachInterrupt(timer, timInterrupt, true);
  timerAlarmWrite(timer, 1000, true);
  timerAlarmEnable(timer);
}

void loop() {
  // put your main code here, to run repeatedly:
}
