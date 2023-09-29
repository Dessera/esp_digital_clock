#include "eclock/button.hpp"

ButtonsEventQueue<3> GlobalEventQueue{};

ButtonState Button::spin_once() {
  uint8_t pin_state = digitalRead(pin);
  if (state_cache == ButtonState::NO_STATE) {
    if (pin_state == LOW) {
      press_counter++;
      if (press_counter >= press_threshold) {
        state_cache = ButtonState::PRESSED;
      }
    } else {
      press_counter = 0;
    }
  } else if (state_cache == ButtonState::PRESSED) {
    if (pin_state == LOW) {
      press_counter++;
      if (press_counter >= long_press_threshold) {
        state_cache = ButtonState::LONG_PRESSED;
      }
    } else {
      press_counter = 0;
      state = ButtonState::PRESSED;
      state_cache = ButtonState::NO_STATE;
    }
  } else if (state_cache == ButtonState::LONG_PRESSED) {
    if (pin_state == HIGH) {
      state = ButtonState::LONG_PRESSED;
      state_cache = ButtonState::NO_STATE;
    }
  }
  return state;
}
