#pragma once
#include <xhash>

namespace hammer {
// "UUID" (universally unique identifier) or GUID is (usually) a 128-bit integer
// used to "uniquely" identify information. In Hazel, even though we use the
// term GUID and UUID, at the moment we're simply using a randomly generated
// 64-bit integer, as the possibility of a clash is low enough for now. This may
// change in the future.
class UUID {
 public:
  UUID();
  UUID(uint64_t uuid);
  UUID(const UUID&) = default;

  operator uint64_t() const { return uuid_; }
 private:
  uint64_t uuid_;
};
}  // namespace hammer

namespace std {
template <>
struct hash<hammer::UUID> {
  std::size_t operator()(const hammer::UUID& uuid) const {
    return hash<uint64_t>()((uint64_t)uuid);
  }
};
}  // namespace std