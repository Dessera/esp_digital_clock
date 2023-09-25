#include <Arduino.h>

#include <clock.hpp>

// TODO: 将数码管操作封装
// ShiftRegister74HC595<1> sr{8, 9, 2};
// const std::array<uint8_t, 10> digits{
//     0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
//     0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111};

void setup() {
  // auto current = GlobalRTClock.now();
  // SerialLogger.fmt_info("Current time: %02d:%02d:%02d", current.Hour(),
  //                       current.Minute(), current.Second());
}

void loop() {
  // put your main code here, to run repeatedly:
  static RtcDateTime stored_time{0};
  if (GlobalRTClockUpdater.last_time() != stored_time) {
    stored_time = GlobalRTClockUpdater.last_time();
    SerialLogger.fmt_info("Current time: %02d:%02d:%02d", stored_time.Hour(),
                          stored_time.Minute(), stored_time.Second());
  }
}
