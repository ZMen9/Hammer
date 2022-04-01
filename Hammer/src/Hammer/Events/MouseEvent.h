#pragma once

#include "Hammer/Events/Event.h"
#include "Hammer/Core/Base.h"
#include "Hammer/Core/MouseCode.h"

namespace hammer {

class  MouseMovedEvent : public Event {
 public:
  MouseMovedEvent(const float x, const float y) : mouse_x_(x), mouse_y_(y) {}

  inline float mouse_x() const { return mouse_x_; }
  inline float mouse_y() const { return mouse_y_; }

  std::string ToString() const override {
    std::stringstream ss;
    ss << "MouseMovedEvent: " << mouse_x_ << "," << mouse_y_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(kMouseMoved)
  EVENT_CLASS_CATEGORY(kEventCategoryMouse | kEventCategoryInput |
                       kEventCategoryMouseButton)

 private:
  float mouse_x_, mouse_y_;
};

class MouseScrolledEvent : public Event {
 public:
  MouseScrolledEvent(const float x, const float y)
      : offset_x_(x), offset_y_(y) {}

  inline float offset_x() const { return offset_x_; }
  inline float offset_y() const { return offset_y_; }

  std::string ToString() const override {
    std::stringstream ss;
    ss << "MouseScrolledEvent: " << offset_x_ << "," << offset_y_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(kMouseScrolled)
  EVENT_CLASS_CATEGORY(kEventCategoryMouse | kEventCategoryInput)

 private:
  float offset_x_, offset_y_;
};

class MouseButtonEvent : public Event {
 public:
  MouseCode button() const { return button_; }
  EVENT_CLASS_CATEGORY(kEventCategoryMouseButton | kEventCategoryInput)

 protected:
  MouseButtonEvent(const MouseCode button) : button_(button) {}

  MouseCode button_;
};

class MouseButtonPressedEvent : public MouseButtonEvent {
 public:
  MouseButtonPressedEvent(const MouseCode button) : MouseButtonEvent(button) {}

  std::string ToString() const override {
    std::stringstream ss;
    ss << "MouseButtonPressedEvent: " << button_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(kMouseButtonPressed)
};

class MouseButtonReleasedEvent : public MouseButtonEvent {
 public:
  MouseButtonReleasedEvent(const MouseCode button) : MouseButtonEvent(button) {}

  std::string ToString() const override {
    std::stringstream ss;
    ss << "MouseButtonReleasedEvent: " << button_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(kMouseButtonReleased)
};

}  // namespace hammer
