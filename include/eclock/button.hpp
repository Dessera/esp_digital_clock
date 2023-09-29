#pragma once

#include <Arduino.h>

#include <array>
#include <cstdint>
#include <memory>
#include <queue>

#include "Print.h"

enum class ButtonState {
  NO_STATE,
  PRESSED,
  LONG_PRESSED,
};

class Button {
 private:
  uint8_t pin{0};

  ButtonState state{ButtonState::NO_STATE};
  ButtonState state_cache{ButtonState::NO_STATE};
  uint8_t press_threshold{0};
  uint8_t long_press_threshold{0};
  uint8_t press_counter{0};

 public:
  Button(uint8_t pin, uint8_t press_threshold, uint8_t long_press_threshold)
      : pin(pin),
        press_threshold(press_threshold),
        long_press_threshold(long_press_threshold) {
    pinMode(pin, INPUT_PULLUP);
  }

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

using ButtonEvent = struct {
  uint8_t pin;
  ButtonState type;
};

template <size_t N>
class ButtonsEventQueue {
 public:
 private:
  std::queue<ButtonEvent> event_queue{};
  std::array<std::unique_ptr<Button>, N> buttons{};
  uint8_t button_count{0};

 public:
  ButtonsEventQueue() = default;
  ~ButtonsEventQueue() = default;

  ButtonsEventQueue(const ButtonsEventQueue&) = delete;
  ButtonsEventQueue& operator=(const ButtonsEventQueue&) = delete;
  ButtonsEventQueue(ButtonsEventQueue&&) noexcept = default;
  ButtonsEventQueue& operator=(ButtonsEventQueue&&) noexcept = default;

  void create_button(uint8_t pin, uint8_t press_threshold,
                     uint8_t long_press_threshold) {
    if (button_count >= N) {
      return;
    }
    buttons[button_count] = std::unique_ptr<Button>(
        new Button(pin, press_threshold, long_press_threshold));
    button_count++;
  }

  void spin_once() {
    for (auto& button : buttons) {
      auto state = button->spin_once();
      if (state != ButtonState::NO_STATE) {
        event_queue.push(ButtonEvent{button->get_pin(), state});
        button->clear_state();
      }
    }
  }

  inline bool empty() const { return event_queue.empty(); }
  ButtonEvent pop_event() {
    auto event = event_queue.front();
    event_queue.pop();
    return event;
  }
};

extern ButtonsEventQueue<3> GlobalEventQueue;
