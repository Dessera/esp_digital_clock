#pragma once

#include <sys/_stdint.h>
#include <sys/_timeval.h>

#include <array>
#include <memory>

#include "eclock/button.hpp"
#include "eclock/widget.hpp"

// class DigitWindow {
//  public:
//   enum class WDotState { OFF, ON, BLINK };
//   struct VisualizerInfo {
//     uint8_t digit_lhs;
//     uint8_t digit_rhs;
//     bool dot_state;
//   };

//   virtual ~DigitWindow() = default;
//   virtual VisualizerInfo get_visualizer_info(time_t tm) = 0;
// };

// class FourDigitWindow : public DigitWindow {
//  private:
//   int* digits_;
//   WDotState dot_state_{WDotState::OFF};

//  public:
//   FourDigitWindow(int* digits, WDotState dot_state)
//       : digits_(digits), dot_state_(dot_state) {}
//   ~FourDigitWindow() override = default;

//   VisualizerInfo get_visualizer_info(time_t tm) override {
//     bool dot_state = false;
//     if (dot_state_ == WDotState::BLINK) {
//       dot_state = (tm % 2) == 0;
//     } else if (dot_state_ == WDotState::OFF) {
//       dot_state = false;
//     } else {
//       dot_state = true;
//     }

//     return VisualizerInfo{static_cast<uint8_t>(*digits_ / 100),
//                           static_cast<uint8_t>(*digits_ % 100), dot_state};
//   }
// };

// class DividedFourDigitWindow : public DigitWindow {
//  private:
//   int* digit_lhs_;
//   int* digit_rhs_;
//   WDotState dot_state_{WDotState::OFF};

//  public:
//   DividedFourDigitWindow(int* digit_lhs, int* digit_rhs, WDotState dot_state)
//       : digit_lhs_(digit_lhs), digit_rhs_(digit_rhs), dot_state_(dot_state)
//       {}
//   ~DividedFourDigitWindow() override = default;

//   VisualizerInfo get_visualizer_info(time_t tm) override {
//     bool dot_state = false;
//     if (dot_state_ == WDotState::BLINK) {
//       dot_state = (tm % 2) == 0;
//     } else if (dot_state_ == WDotState::OFF) {
//       dot_state = false;
//     } else {
//       dot_state = true;
//     }

//     return VisualizerInfo{static_cast<uint8_t>(*digit_lhs_),
//                           static_cast<uint8_t>(*digit_rhs_), dot_state};
//   }
// };

class UserInterface {
 public:
  virtual ~UserInterface() = default;

  virtual void on_load(UserInterface* previous_ui){};
  virtual void on_unload(UserInterface* next_ui){};
  virtual void on_event(const ButtonEvent& event){};
  virtual void on_tick() = 0;
};

class UserInterfaceManager {
 private:
  UserInterface* ui_{nullptr};

 public:
  explicit UserInterfaceManager(UserInterface* ui) : ui_(ui) {
    ui_->on_load(nullptr);
  }

  inline void on_event(const ButtonEvent& event) {
    if (ui_ != nullptr) {
      ui_->on_event(event);
    }
  }

  inline void on_tick() {
    if (ui_ != nullptr) {
      ui_->on_tick();
    }
  }

  inline void set_ui(UserInterface* ui) {
    if (ui_ != nullptr) {
      ui_->on_unload(ui);
    }
    ui_ = ui;
    ui_->on_load(ui_);
  }
};

class TimeDisplayUI : public UserInterface {
 private:
  time_t current_tm{0};
  time_t last_tm{0};
  tm time_parsed{};
  uint8_t current_digit_window{0};

  std::array<std::unique_ptr<DigitWindow>, 4> digit_windows;

 public:
  TimeDisplayUI();
  ~TimeDisplayUI() override = default;

  void on_tick() override;
  void on_event(const ButtonEvent& event) override;

  void format_time();
};

class TimeEditUI : public UserInterface {
 private:
  time_t last_tm{0};
  struct tm init_time {};
  std::array<std::unique_ptr<DigitWindow>, 6> digit_windows;
  uint8_t current_digit_window{0};

  void increase_current_time();

 public:
  TimeEditUI();

  void on_tick() override;
  void on_event(const ButtonEvent& event) override;
  void on_load(UserInterface* previous_ui) override;
  void on_unload(UserInterface* next_ui) override;
};

class AlarmUI : public UserInterface {
 private:
  time_t last_tm{0};
  struct tm init_time {};
  std::array<std::unique_ptr<DigitWindow>, 6> digit_windows;
  uint8_t current_digit_window{0};

  void increase_current_time();

 public:
  void on_tick() override;
  void on_event(const ButtonEvent& event) override;
  void on_load(UserInterface* previous_ui) override;
  void on_unload(UserInterface* next_ui) override;
};

class ConfigUI : public UserInterface {
 private:
  int init_brightness{0};
  int beep_strength{0};

 public:
  ConfigUI();

  void on_tick() override;
  void on_event(const ButtonEvent& event) override;
  void on_load(UserInterface* previous_ui) override;
  void on_unload(UserInterface* next_ui) override;
};

extern UserInterfaceManager GlobalUIManager;

extern TimeDisplayUI TimeDisplay;
extern TimeEditUI TimeEdit;
