#include <Arduino.h>

#include "eclock/button.hpp"
#include "eclock/clock.hpp"
#include "eclock/config.hpp"

// #include "button.hpp"
// #include "clock.hpp"
// #include "interface.hpp"

TaskHandle_t eclock_update_screen_task_handle;
void eclock_update_screen_task(void* arg) {
  while (true) {
    GlobalRTClockVisualizer.sync_buffer();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
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

    // when user interface is implemented, this is where the event should be sent

    // Serial.printf("Button %d %s\n", event.first, event.second ==
    // ButtonState::PRESSED ? "pressed" : "long pressed");
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
  // Serial.begin(LOGGER_USING_BAUD);
  // GlobalButtonGroup.create_button(BUTTON_1_USING_PIN, BUTTON_PRESS_THRESHOLD,
  //                                 BUTTON_LONG_PRESS_THRESHOLD);
  // GlobalButtonGroup.create_button(BUTTON_2_USING_PIN, BUTTON_PRESS_THRESHOLD,
  //                                 BUTTON_LONG_PRESS_THRESHOLD);
  // GlobalButtonGroup.create_button(BUTTON_3_USING_PIN, BUTTON_PRESS_THRESHOLD,
  //                                 BUTTON_LONG_PRESS_THRESHOLD);
  // GlobalButtonGroup.begin();
  // GlobalRTClockVisualizer.begin();
}

void loop() {
  // GlobalInterface->display(GlobalButtonGroup.get_current_button(),
  //                          GlobalButtonGroup.get_current_state(),
  //                          GlobalRTClockUpdater.last_time());
}
