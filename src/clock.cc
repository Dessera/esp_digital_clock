#include "clock.hpp"

#include "logger.hpp"

template <uint8_t IO, uint8_t CLK, uint8_t CE>
RTClock<IO, CLK, CE>::RTClock() {
  auto& logger = SerialLogger::instance();
  m_rtc.Begin();
  logger.fmt_info("RTC initializing, compile time: %s %s\n", __DATE__,
                  __TIME__);
  if (!m_rtc.IsDateTimeValid()) {
    logger.warn("RTC lost confidence in the DateTime!\n");
    m_rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
  }

  m_rtc.SetIsRunning(true);
  logger.info("RTC initialized\n");
}

template <uint8_t IO, uint8_t CLK, uint8_t CE>
RtcDateTime RTClock<IO, CLK, CE>::now() {
  return m_rtc.GetDateTime();
}
