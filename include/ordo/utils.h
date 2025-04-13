#ifndef ORDO_UTILS_H
#define ORDO_UTILS_H

#include <functional>

namespace ordo {
  template <typename T>
  inline void hash_combine(std::size_t& seed, const T& value) {
    std::hash<T> hasher;
    seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
} // namespace ordo

#endif // ORDO_UTILS_H