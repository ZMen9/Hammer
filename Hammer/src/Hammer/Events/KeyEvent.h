#pragma once

#include "Hammer/Events/Event.h"

namespace hammer {

class KeyEvent : public Event {
 public:
  int keycode() const { return keycode_; }

  EVENT_CLASS_CATEGORY(kEventCategoryKeyboard | kEventCategoryInput)
 protected:
  KeyEvent(int keycore) : keycode_(keycore) {}

  int keycode_;
};

class KeyPressedEvent : public KeyEvent {
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

class KeyReleasedEvent : public KeyEvent {
 public:
  KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

  std::string ToString() const override {
    std::stringstream ss;
    ss << "KeyReleasedEvent: " << keycode_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(kKeyReleased)
};

class KeyTypedEvent : public KeyEvent {
 public:
  KeyTypedEvent(int keycode)
      : KeyEvent(keycode){}


  std::string ToString() const override {
    std::stringstream ss;
    ss << "KeyTypedEvent: " << keycode_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(kKeyTyped)
};


}  // namespace hammer