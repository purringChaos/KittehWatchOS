#pragma once
#include <cstddef>

namespace Pinetime {
namespace Drivers {
class BufferProvider {
public:
  virtual bool GetNextBuffer(uint8_t **buffer, size_t &size) = 0;
};
} // namespace Drivers
} // namespace Pinetime