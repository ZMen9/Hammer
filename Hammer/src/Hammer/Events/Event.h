#pragma once
#include "Hammer/Core.h"
#include "hmpch.h"

namespace hammer {

enum class EventType {
  kNone = 0,
  //
  kWindowClose,
  kWindowResize,
  kWindowFocus,
  kWindowLostFocus,
  kWindowMoved,
  //
  kAppTick,
  kAppUpdate,
  kAppRender,
  //
  kKeyPressed,
  kKeyReleased,
  //
  kMouseButtonPressed,
  kMouseButtonReleased,
  kMouseMoved,
  kMouseScrolled

};

enum EventCategory {
  kNone = 0,
  KEventCategoryApplication = BIT(0),
  kEventCategoryInput = BIT(1),
  kEventCategoryKeyboard = BIT(2),
  kEventCategoryMouse = BIT(3),
  kEventCategoryMouseButton = BIT(4)
};

#define EVENT_CLASS_TYPE(type)                                                \
  static EventType GetStaticType() { return EventType::##type; }              \
  virtual EventType GetEventType() const override { return GetStaticType(); } \
  virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
  virtual int GetCategoryFlags() const override { return category; }

class HAMMER_API Event {
  friend class EventDispatcher;

 public:
  virtual EventType GetEventType() const = 0;
  // GetName() sure only for debug mode
  virtual const char* GetName() const = 0;
  virtual int GetCategoryFlags() const = 0;
  // sure only for debug mode
  virtual std::string ToString() const { return GetName(); }

  inline bool IsInCategory(EventCategory category) {
    /*using type = std::underlying_type<EventCategory>::type;
    return GetCategoryFlags() & static_cast<type>(category);*/
    return GetCategoryFlags() & category;
  }

 public:
  // is the event handled?
  // if the event isn't wanted to be propagated in various layers,this variable
  // will also be useful
  bool handled_ = false;
};

class EventDispatcher {
  template <typename T>
  using EventFn = std::function<bool(T&)>;

 public:
  EventDispatcher(Event& event) : event_(event) {}

  template <typename T>
  bool Dispatch(EventFn<T> func) {
    if (event_.GetEventType() == T::GetStaticType()) {
      event_.handled_ = func(*(T*)&event_);
      return true;
    }
    return false;
  }

 private:
  Event& event_;
};

inline std::ostream& operator<<(std::ostream& os, const Event& e) {
  return os << e.ToString();
}

}  // namespace hammer
