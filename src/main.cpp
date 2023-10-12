#include <Arduino.h>

#include "eclock/button.hpp"
#include "eclock/clock.hpp"
#include "eclock/config.hpp"
#include "eclock/interface.hpp"
#include "esp32-hal.h"

// #include "button.hpp"
// #include "clock.hpp"
// #include "interface.hpp"

TaskHandle_t eclock_update_screen_task_handle;
void eclock_update_screen_task(void* arg) {
  while (true) {
    if (GlobalRTClockVisualizer.is_buffer_edited()) {
      GlobalRTClockVisualizer.sync_buffer();
      GlobalRTClockVisualizer.clear_buffer_edited_flag();
    }
    if (GlobalRTClockVisualizer.is_config_edited()) {
      GlobalRTClockVisualizer.sync_config();
      GlobalRTClockVisualizer.clear_config_edited_flag();
    }

    // delay 0.5s
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

TaskHandle_t eclock_push_button_task_handle;
void eclock_push_button_task(void* arg) {
  while (true) {
    GlobalEventQueue.spin_once();
    vTaskDelay(10);
  }
}

TaskHandle_t eclock_pop_button_task_handle;
void eclock_poll_button_task(void* arg) {
  while (true) {
    // wait for button event
    while (GlobalEventQueue.empty()) {
      vTaskDelay(10);
    }
    auto event = GlobalEventQueue.pop_event();
    GlobalUIManager.on_event(event);
  }
}

TaskHandle_t eclock_ui_manager_task_handle;
void eclock_ui_manager_task(void* arg) {
  while (true) {
    GlobalUIManager.on_tick();
    vTaskDelay(10);
  }
}

TaskHandle_t eclock_alarm_task_handle;
void eclock_alarm_task(void* arg) {
  while (true) {
    GlobalRTClockAlarmWatcher.spin_once(GlobalRTClock.get_time());
  }
}

void setup() {
  Serial.begin(LOGGER_BAUD);

  GlobalEventQueue.create_button(BTN_1_PIN, BTN_PRESS_THRESHOLD,
                                 BTN_LONG_PRESS_THRESHOLD);
  GlobalEventQueue.create_button(BTN_2_PIN, BTN_PRESS_THRESHOLD,
                                 BTN_LONG_PRESS_THRESHOLD);
  GlobalEventQueue.create_button(BTN_3_PIN, BTN_PRESS_THRESHOLD,
                                 BTN_LONG_PRESS_THRESHOLD);

  xTaskCreate(eclock_update_screen_task, "eclock_update_screen_task", 1024,
              nullptr, 1, &eclock_update_screen_task_handle);
  xTaskCreate(eclock_push_button_task, "eclock_push_button_task", 1024, nullptr,
              1, &eclock_push_button_task_handle);
  xTaskCreate(eclock_poll_button_task, "eclock_poll_button_task", 1024, nullptr,
              1, &eclock_pop_button_task_handle);
  xTaskCreate(eclock_ui_manager_task, "eclock_ui_manager_task", 1024, nullptr,
              1, &eclock_ui_manager_task_handle);
  xTaskCreate(eclock_alarm_task, "eclock_alarm_task", 1024, nullptr, 1,
              &eclock_alarm_task_handle);
}

void loop() {}
