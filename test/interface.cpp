#include "interface.hpp"

TimeDisplayInterface* TimeDisplay = new TimeDisplayInterface();
TimeSetInterface* TimeSet = new TimeSetInterface();
UserInterface* GlobalInterface = TimeDisplay;

void TimeDisplayInterface::display(ButtonGroup::ButtonHandle current_button,
                                   ButtonState current_state,
                                   const RtcDateTime& tm) {
  if (current_button != -1) {
    if (current_button == BUTTON_2_USING_PIN) {
      set_next_display_state();
    } else if (current_button == BUTTON_3_USING_PIN) {
      set_pre_display_state();
    } else if (current_button == BUTTON_1_USING_PIN &&
               current_state == ButtonState::LONG_PRESSED) {
      GlobalInterface = TimeSet;
      TimeSet->set_time_of_enter(tm);
    }
    GlobalButtonGroup.clear();
  } else {
    if (tm.TotalSeconds() != last_time.TotalSeconds()) {
      last_time = tm;
      switch (state) {
        case DisplayState::YEAR:
          GlobalRTClockVisualizer.set_visualize_year(tm.Year());
          break;
        case DisplayState::DATE:
          GlobalRTClockVisualizer.set_visualize_month_day(tm.Month(), tm.Day());
          break;
        case DisplayState::TIME:
          GlobalRTClockVisualizer.set_visualize_hour_minute(tm.Hour(),
                                                            tm.Minute());
          break;
        case DisplayState::TIME_DETAIL:
          GlobalRTClockVisualizer.set_pair_of_number(tm.Minute(), tm.Second());
          GlobalRTClockVisualizer.set_dot(tm.Second() % 2);
          break;
      }
    }
  }
}

void TimeSetInterface::display(ButtonGroup::ButtonHandle current_button,
                               ButtonState current_state,
                               const RtcDateTime& tm) {
  if (current_button != -1) {
    if (current_button == BUTTON_1_USING_PIN) {
      if (current_state == ButtonState::PRESSED) {
        time_of_enter_to_current();
        GlobalInterface = TimeDisplay;
      }

    } else if (current_button == BUTTON_2_USING_PIN) {
      set_next_edit_state();
    } else if (current_button == BUTTON_3_USING_PIN) {
      time_of_enter[current_index.first].first =
          (time_of_enter[current_index.first].first + 1) %
          time_of_enter[current_index.first].second;
    }
    GlobalButtonGroup.clear();
  } else {
    set_current_setting_time(tm.Second() % 2);
  }
}
