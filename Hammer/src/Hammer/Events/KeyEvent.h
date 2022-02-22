#pragma once

#include "Event.h"

namespace hammer {

class HAMMER_API KeyEvent : public Event {
 public:
  int keycode() const { return keycode_; }

  EVENT_CLASS_CATEGORY(kEventCategoryKeyboard | kEventCategoryInput)
 protected:
  KeyEvent(int keycore) : keycode_(keycore) {}

  int keycode_;
};

class HAMMER_API KeyPressedEvent : public KeyEvent {
 public:
  KeyPressedEvent(int keycode, int repeat_count)
      : KeyEvent(keycode), repeat_count_(repeat_count) {}

  inline int repeat_count() const { return repeat_count_; }

  std::string ToString() const override {
    std::stringstream ss;
    ss << "KeyPressedEvent: " << keycode_ << " (" << repeat_count_
       << " repeats)";
    return ss.str();
  }

  EVENT_CLASS_TYPE(kKeyPressed)
 private:
  int repeat_count_;
};

class HAMMER_API KeyReleasedEvent : public KeyEvent {
 public:
  KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

  std::string ToString() const override {
    std::stringstream ss;
    ss << "KeyReleasedEvent: " << keycode_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(kKeyReleased)
};

}  // namespace hammer