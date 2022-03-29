#pragma once 

#include "Hammer/Events/Event.h"
#include "Hammer/Core/Base.h"

namespace hammer {

class WindowCloseEvent : public Event {
 public:
  WindowCloseEvent() = default;
  
  EVENT_CLASS_TYPE(kWindowClose)
  EVENT_CLASS_CATEGORY(KEventCategoryApplication)

};

class WindowResizeEvent : public Event {
 public:
  WindowResizeEvent(unsigned int width, unsigned int height)
      : width_(width), height_(height) {}

  inline unsigned int width() const { return width_; }
  inline unsigned int height() const { return height_; }

  std::string ToString() const override {
    std::stringstream ss;
    ss << "WindowResizeEvent: " << width_ << "," << height_;
    return ss.str();
  }

  EVENT_CLASS_TYPE(kWindowResize)
  EVENT_CLASS_CATEGORY(KEventCategoryApplication)

 private:
  unsigned int width_, height_;
};

class AppTickEvent : public Event {
 public:
  AppTickEvent() = default;

  EVENT_CLASS_TYPE(kAppTick)
  EVENT_CLASS_CATEGORY(KEventCategoryApplication)
};

class AppUpdateEvent : public Event {
 public:
  AppUpdateEvent() = default;

  EVENT_CLASS_TYPE(kAppUpdate)
  EVENT_CLASS_CATEGORY(KEventCategoryApplication)
};

class AppRenderEvent : public Event {
 public:
  AppRenderEvent() = default;

  EVENT_CLASS_TYPE(kAppRender)
  EVENT_CLASS_CATEGORY(KEventCategoryApplication)
};

} //namespace hammer