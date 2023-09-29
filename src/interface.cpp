#include "eclock/interface.hpp"

#include <ctime>
#include <memory>

#include "eclock/button.hpp"
#include "eclock/clock.hpp"
#include "eclock/config.hpp"

UserInterfaceManager GlobalUIManager{&TimeDisplay};
TimeDisplayUI TimeDisplay{};
TimeEditUI TimeEdit{};

TimeDisplayUI::TimeDisplayUI() {
  digit_windows[0] = std::unique_ptr<DigitWindow>(
      new Window4Digit(&(time_parsed.tm_year), LEDState::ON, LEDState::OFF));
  digit_windows[1] = std::unique_ptr<DigitWindow>(
      new Window2X2Digit(&(time_parsed.tm_mon), &(time_parsed.tm_mday),
                         LEDState::ON, LEDState::ON, LEDState::OFF));
  digit_windows[2] = std::unique_ptr<DigitWindow>(
      new Window2X2Digit(&(time_parsed.tm_hour), &(time_parsed.tm_min),
                         LEDState::ON, LEDState::ON, LEDState::BLINK));
  digit_windows[3] = std::unique_ptr<DigitWindow>(
      new Window2X2Digit(&(time_parsed.tm_min), &(time_parsed.tm_sec),
                         LEDState::ON, LEDState::ON, LEDState::BLINK));
}

void TimeDisplayUI::on_event(const ButtonEvent& event) {
  if (event.pin == BTN_2_PIN) {
    current_digit_window = (current_digit_window + 1) % 4;
  } else if (event.pin == BTN_1_PIN) {
    if (event.type == ButtonState::LONG_PRESSED) {
      GlobalUIManager.set_ui(&TimeEdit);
    } else if (event.type == ButtonState::PRESSED) {
      GlobalRTClockAlarmWatcher.add_alarm(current_tm + 5);
    }
  }
}

void TimeDisplayUI::on_tick() {
  this->current_tm = GlobalRTClock.get_time();
  if (this->current_tm == this->last_tm) {
    return;
  }
  this->last_tm = this->current_tm;
  GlobalRTClock.get_tm_struct(&this->time_parsed);

  GlobalRTClockVisualizer.set_lnum(
      this->digit_windows[this->current_digit_window]->get_lnum(
          this->current_tm));
  GlobalRTClockVisualizer.set_rnum(
      this->digit_windows[this->current_digit_window]->get_rnum(
          this->current_tm));
  GlobalRTClockVisualizer.set_wdot(
      this->digit_windows[this->current_digit_window]->get_wdot(
          this->current_tm));
}

void TimeDisplayUI::format_time() {
  time_parsed.tm_year += 1900;
  time_parsed.tm_mon += 1;
  time_parsed.tm_wday += 1;
}

TimeEditUI::TimeEditUI() {
  digit_windows[0] = std::unique_ptr<DigitWindow>(
      new Window4Digit(&(init_time.tm_year), LEDState::BLINK, LEDState::OFF));
  digit_windows[1] = std::unique_ptr<DigitWindow>(
      new Window2X2Digit(&(init_time.tm_mon), &(init_time.tm_mday),
                         LEDState::BLINK, LEDState::ON, LEDState::OFF));
  digit_windows[2] = std::unique_ptr<DigitWindow>(
      new Window2X2Digit(&(init_time.tm_mon), &(init_time.tm_mday),
                         LEDState::ON, LEDState::BLINK, LEDState::OFF));
  digit_windows[3] = std::unique_ptr<DigitWindow>(
      new Window2X2Digit(&(init_time.tm_hour), &(init_time.tm_min),
                         LEDState::BLINK, LEDState::ON, LEDState::BLINK));
  digit_windows[4] = std::unique_ptr<DigitWindow>(
      new Window2X2Digit(&(init_time.tm_hour), &(init_time.tm_min),
                         LEDState::ON, LEDState::BLINK, LEDState::BLINK));
  digit_windows[5] = std::unique_ptr<DigitWindow>(
      new Window2X2Digit(&(init_time.tm_min), &(init_time.tm_sec), LEDState::ON,
                         LEDState::BLINK, LEDState::BLINK));
}

void TimeEditUI::on_load(UserInterface* previous_ui) {
  GlobalRTClock.get_tm_struct(&init_time);
}

void TimeEditUI::on_unload(UserInterface* next_ui) {
  GlobalRTClock.set_tm_struct(&init_time);
}

void TimeEditUI::on_event(const ButtonEvent& event) {
  if (event.pin == BTN_3_PIN) {
    increase_current_time();
  } else if (event.pin == BTN_2_PIN) {
    current_digit_window = (current_digit_window + 1) % 6;
  } else if (event.pin == BTN_1_PIN) {
    if (event.type == ButtonState::PRESSED) {
      GlobalRTClock.set_tm_struct(&init_time);
    }
  }
}

void TimeEditUI::on_tick() {
  time_t current_tm = GlobalRTClock.get_time();
  if (current_tm == this->last_tm) {
    return;
  }
  GlobalRTClockVisualizer.set_lnum(
      this->digit_windows[this->current_digit_window]->get_lnum(current_tm));
  GlobalRTClockVisualizer.set_rnum(
      this->digit_windows[this->current_digit_window]->get_rnum(current_tm));
  GlobalRTClockVisualizer.set_wdot(
      this->digit_windows[this->current_digit_window]->get_wdot(current_tm));
}

void TimeEditUI::increase_current_time() {
  switch (current_digit_window) {
    case 0:
      init_time.tm_year += 1;
      init_time.tm_year = init_time.tm_year > 9999 ? 0 : init_time.tm_year;
      break;
    case 1:
      init_time.tm_mon += 1;
      init_time.tm_mon = init_time.tm_mon > 12 ? 1 : init_time.tm_mon;
      break;
    case 2:
      init_time.tm_mday += 1;
      init_time.tm_mday = init_time.tm_mday > 31 ? 1 : init_time.tm_mday;
      break;
    case 3:
      init_time.tm_hour += 1;
      init_time.tm_hour = init_time.tm_hour > 23 ? 0 : init_time.tm_hour;
      break;
    case 4:
      init_time.tm_min += 1;
      init_time.tm_min = init_time.tm_min > 59 ? 0 : init_time.tm_min;
      break;
    case 5:
      init_time.tm_sec += 1;
      init_time.tm_sec = init_time.tm_sec > 59 ? 0 : init_time.tm_sec;
      break;
    default:
      break;
  }
}
