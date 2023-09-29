#pragma once

#include <sys/_stdint.h>

#include <array>
#include <ctime>

#include "assert.h"

enum class LEDState { ON, OFF, BLINK };

class DigitWindow {
 public:
  virtual ~DigitWindow() = default;
  virtual int get_lnum(time_t tm) = 0;
  virtual int get_rnum(time_t tm) = 0;
  virtual bool get_wdot(time_t tm) = 0;
};

class Window4Digit : public DigitWindow {
 private:
  int* digits_;
  LEDState num_state_{LEDState::ON};
  LEDState dot_state_{LEDState::OFF};

 public:
  Window4Digit(int* digits, LEDState num_state, LEDState dot_state)
      : digits_(digits), num_state_(num_state), dot_state_(dot_state) {
    assert(digits_ != nullptr);
  }
  ~Window4Digit() override = default;

  int get_lnum(time_t tm) override {
    if (num_state_ == LEDState::BLINK) {
      return (tm % 2) == 0 ? *digits_ / 100 : -1;
    } else if (num_state_ == LEDState::OFF) {
      return -1;
    } else {
      return *digits_ / 100;
    }
  }

  int get_rnum(time_t tm) override {
    if (num_state_ == LEDState::BLINK) {
      return (tm % 2) == 0 ? *digits_ % 100 : -1;
    } else if (num_state_ == LEDState::OFF) {
      return -1;
    } else {
      return *digits_ % 100;
    }
  }

  bool get_wdot(time_t tm) override {
    if (dot_state_ == LEDState::BLINK) {
      return (tm % 2) == 0;
    } else if (dot_state_ == LEDState::OFF) {
      return false;
    } else {
      return true;
    }
  }
};

class Window2X2Digit : public DigitWindow {
 private:
  int* digit_lhs_;
  int* digit_rhs_;
  LEDState num_lhs_state_{LEDState::ON};
  LEDState num_rhs_state_{LEDState::ON};
  LEDState dot_state_{LEDState::OFF};

 public:
  Window2X2Digit(int* digit_lhs, int* digit_rhs, LEDState num_lhs_state,
                 LEDState num_rhs_state, LEDState dot_state)
      : digit_lhs_(digit_lhs),
        digit_rhs_(digit_rhs),
        num_lhs_state_(num_lhs_state),
        num_rhs_state_(num_rhs_state),
        dot_state_(dot_state) {
    assert(digit_lhs_ != nullptr);
    assert(digit_rhs_ != nullptr);
  }
  ~Window2X2Digit() override = default;

  int get_lnum(time_t tm) override {
    if (num_lhs_state_ == LEDState::BLINK) {
      return (tm % 2) == 0 ? *digit_lhs_ : -1;
    } else if (num_lhs_state_ == LEDState::OFF) {
      return -1;
    } else {
      return *digit_lhs_;
    }
  }

  int get_rnum(time_t tm) override {
    if (num_rhs_state_ == LEDState::BLINK) {
      return (tm % 2) == 0 ? *digit_rhs_ : -1;
    } else if (num_rhs_state_ == LEDState::OFF) {
      return -1;
    } else {
      return *digit_rhs_;
    }
  }

  bool get_wdot(time_t tm) override {
    if (dot_state_ == LEDState::BLINK) {
      return (tm % 2) == 0;
    } else if (dot_state_ == LEDState::OFF) {
      return false;
    } else {
      return true;
    }
  }
};
