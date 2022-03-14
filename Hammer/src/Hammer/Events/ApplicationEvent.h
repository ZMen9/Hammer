#pragma once 

#include "Event.h"
#include "Hammer/Core/Core.h"

namespace hammer {

class HAMMER_API WindowCloseEvent : public Event {
 public:
  WindowCloseEvent() {}
  
  EVENT_CLASS_TYPE(kWindowClose)
  EVENT_CLASS_CATEGORY(KEventCategoryApplication)

};

class HAMMER_API WindowResizeEvent : public Event {
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

class HAMMER_API AppTickEvent : public Event {
 public:
  AppTickEvent() {}

  EVENT_CLASS_TYPE(kAppTick)
  EVENT_CLASS_CATEGORY(KEventCategoryApplication)
};

class HAMMER_API AppUpdateEvent : public Event {
 public:
  AppUpdateEvent() {}

  EVENT_CLASS_TYPE(kAppUpdate)
  EVENT_CLASS_CATEGORY(KEventCategoryApplication)
};

class HAMMER_API AppRenderEvent : public Event {
 public:
  AppRenderEvent() {}

  EVENT_CLASS_TYPE(kAppRender)
  EVENT_CLASS_CATEGORY(KEventCategoryApplication)
};

} //namespace hammer