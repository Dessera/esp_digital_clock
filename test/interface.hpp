#pragma once

#include <Arduino.h>

#include <array>
#include <cstdint>
#include <type_traits>

#include "RtcDS1302.h"
#include "RtcDateTime.h"
#include "button.hpp"
#include "clock.hpp"
#include "config.hpp"

class UserInterface {
 public:
  virtual void display(ButtonGroup::ButtonHandle current_button,
                       ButtonState current_state, const RtcDateTime& tm) = 0;
};

class TimeDisplayInterface : public UserInterface {
 private:
  RtcDateTime last_time{0};
  enum class DisplayState {
    YEAR = 0,
    DATE,
    TIME,
    TIME_DETAIL,
  } state{DisplayState::TIME_DETAIL};

  void set_next_display_state() {
    switch (state) {
      case DisplayState::YEAR:
        state = DisplayState::DATE;
        break;
      case DisplayState::DATE:
        state = DisplayState::TIME;
        break;
      case DisplayState::TIME:
        state = DisplayState::TIME_DETAIL;
        break;
      case DisplayState::TIME_DETAIL:
        state = DisplayState::YEAR;
        break;
    }
  }

  void set_pre_display_state() {
    switch (state) {
      case DisplayState::YEAR:
        state = DisplayState::TIME_DETAIL;
        break;
      case DisplayState::DATE:
        state = DisplayState::YEAR;
        break;
      case DisplayState::TIME:
        state = DisplayState::DATE;
        break;
      case DisplayState::TIME_DETAIL:
        state = DisplayState::TIME;
        break;
    }
  }

 public:
  void display(ButtonGroup::ButtonHandle current_button,
               ButtonState current_state, const RtcDateTime& tm) override;
};

class TimeSetInterface : public UserInterface {
 public:
  using ConfigPair = std::pair<uint8_t, uint8_t>;
  using Position = std::pair<uint8_t, uint8_t>;
  using ConfigArray = std::array<ConfigPair, 7>;

 private:
  ConfigArray time_of_enter{};
  Position current_index{0, 0};

  void set_next_edit_state() {
    current_index.first = (current_index.first + 1) % 7;
    current_index.second++;
    if (current_index.first != 6) {
      current_index.second = current_index.first;
      current_index.second &= 0xFE;
    } else {
      current_index.second = 5;
    }
  }

  void set_current_setting_time(bool display_off) {
    static uint8_t b1, b2, b3, b4;

    uint8_t intvl = time_of_enter[current_index.second].first;
    uint8_t intvr = time_of_enter[current_index.second + 1].first;
    bool left_flag = current_index.first % 2 == 0;
    if (current_index.first == 6) {
      left_flag = false;
    }

    // 选择闪烁位
    if (display_off) {
      if (left_flag) {
        b1 = b2 = 10;
        b3 = intvr / 10;
        b4 = intvr % 10;
      } else {
        b1 = intvl / 10;
        b2 = intvl % 10;
        b3 = b4 = 10;
      }
    } else {
      b1 = intvl / 10;
      b2 = intvl % 10;
      b3 = intvr / 10;
      b4 = intvr % 10;
    }

    GlobalRTClockVisualizer.set_buf(b1, b2, b3, b4);
    if (current_index.first > 3) {
      if (display_off) {
        GlobalRTClockVisualizer.set_dot(false);
      } else {
        GlobalRTClockVisualizer.set_dot(true);
      }
    } else {
      GlobalRTClockVisualizer.set_dot(false);
    }
  }

 public:
  void set_time_of_enter(const RtcDateTime& tm) {
    time_of_enter[0].first = tm.Year() / 100;
    time_of_enter[0].second = 99;
    time_of_enter[1].first = tm.Year() % 100;
    time_of_enter[1].second = 99;
    time_of_enter[2].first = tm.Month();
    time_of_enter[2].second = 12;
    time_of_enter[3].first = tm.Day();
    time_of_enter[3].second = 31;
    time_of_enter[4].first = tm.Hour();
    time_of_enter[4].second = 24;
    time_of_enter[5].first = tm.Minute();
    time_of_enter[5].second = 60;
    time_of_enter[6].first = tm.Second();
    time_of_enter[6].second = 60;
  }

  void time_of_enter_to_current() {
    uint32_t tv_sec = 0;
    RtcDateTime tm{static_cast<uint16_t>(time_of_enter[0].first * 100 +
                                         time_of_enter[1].first),
                   time_of_enter[2].first,
                   time_of_enter[3].first,
                   time_of_enter[4].first,
                   time_of_enter[5].first,
                   time_of_enter[6].first};
    GlobalRTClock.set_time(tm.Unix32Time());
    GlobalRTClockUpdater.set_time(tm.Unix32Time());
  }

  void display(ButtonGroup::ButtonHandle current_button,
               ButtonState current_state, const RtcDateTime& tm) override;
};

extern TimeDisplayInterface* TimeDisplay;
extern TimeSetInterface* TimeSet;
extern UserInterface* AlarmSet;
extern UserInterface* Settings;

extern UserInterface* GlobalInterface;
