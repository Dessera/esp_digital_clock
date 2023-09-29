#pragma once
#include <Arduino.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "config.hpp"

enum class ButtonState {
  NO_STATE,
  PRESSED,
  LONG_PRESSED,
};

class Button {
  friend class ButtonGroup;

 private:
  uint8_t pin{0};

  ButtonState state{ButtonState::NO_STATE};
  ButtonState state_cache{ButtonState::NO_STATE};
  uint8_t press_threshold{0};
  uint8_t long_press_threshold{0};
  uint8_t press_counter{0};

  Button(uint8_t pin, uint8_t press_threshold, uint8_t long_press_threshold)
      : pin(pin),
        press_threshold(press_threshold),
        long_press_threshold(long_press_threshold) {
    pinMode(pin, INPUT_PULLUP);
  }

 public:
  ~Button() = default;

  Button(const Button&) = default;
  Button& operator=(const Button&) = default;
  Button(Button&&) = default;
  Button& operator=(Button&&) = default;

  ButtonState spin_once();

  inline void clear_state() {
    state = ButtonState::NO_STATE;
    state_cache = ButtonState::NO_STATE;
  }

  inline uint8_t get_pin() const { return pin; }
};

void btn_task(void* param);

class ButtonGroup {
 public:
  using ButtonHandle = int8_t;

 private:
  std::vector<Button> buttons{};

  ButtonHandle current_button{-1};
  ButtonState current_state{ButtonState::NO_STATE};

  TaskHandle_t btn_task_handle{nullptr};

 public:
  ButtonGroup() = default;
  ~ButtonGroup() = default;

  ButtonGroup(const ButtonGroup&) = delete;
  ButtonGroup& operator=(const ButtonGroup&) = delete;
  ButtonGroup(ButtonGroup&&) = delete;
  ButtonGroup& operator=(ButtonGroup&&) = delete;

  void create_button(uint8_t pin, uint8_t press_threshold,
                     uint8_t long_press_threshold) {
    buttons.push_back(Button{pin, press_threshold, long_press_threshold});
  }

  void spin_once() {
    ButtonState state{ButtonState::NO_STATE};
    for (int i = 0; i < buttons.size(); i++) {
      state = buttons[i].spin_once();
      if (state != ButtonState::NO_STATE) {
        current_button = buttons[i].get_pin();
        current_state = state;
        break;
      }
    }
  }

  inline ButtonHandle get_current_button() const { return current_button; }
  inline ButtonState get_current_state() const { return current_state; }

  void clear() {
    std::for_each(buttons.begin(), buttons.end(),
                  [](Button& btn) { btn.clear_state(); });
    current_button = -1;
    current_state = ButtonState::NO_STATE;
  }

  void begin() {
    xTaskCreate(btn_task, "btn_task", 512, nullptr, 1, &btn_task_handle);
  }
};

extern ButtonGroup GlobalButtonGroup;
