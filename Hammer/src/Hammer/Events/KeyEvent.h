#pragma once

#include "Hammer/Events/Event.h"
#include "Hammer/Core/KeyCode.h"
namespace hammer {

class KeyEvent : public Event {
 public:
  KeyCode keycode() const { return keycode_; }

  EVENT_CLASS_CATEGORY(kEventCategoryKeyboard | kEventCategoryInput)
 protected:
  KeyEvent(const KeyCode keycode) : keycode_(keycode) {}

  KeyCode keycode_;
};

class KeyPressedEvent : public KeyEvent {
 public:
  KeyPressedEvent(const KeyCode keycode, const uint16_t repeat_count)
      : KeyEvent(keycode), repeat_count_(repeat_count) {}

  uint16_t repeat_count() const { return repeat_count_; }

  std::string ToString() const override {
    std::stringstream ss;
    ss << "KeyPressedEvent: " << keycode_ << " (" << repeat_count_
       << " repeats)";
    return ss.str();
  }

  EVENT_CLASS_TYPE(kKeyPressed)
 private:
  uint16_t repeat_count_;
};

class KeyReleasedEvent : public KeyEvent {
 public:
  KeyReleasedEvent(const KeyCode keycode) : KeyEvent(keycode) {}

  std::string ToString() const override {
    std::stringstream ss;
    ss << "KeyReleasedEvent: " << keycode_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(kKeyReleased)
};

class KeyTypedEvent : public KeyEvent {
 public:
  KeyTypedEvent(const KeyCode keycode)
      : KeyEvent(keycode){}


  std::string ToString() const override {
    std::stringstream ss;
    ss << "KeyTypedEvent: " << keycode_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(kKeyTyped)
};


}  // namespace hammer
