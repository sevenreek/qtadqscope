#ifndef UTIL_H
#define UTIL_H
#include <algorithm>
template <typename T>
T clip(const T& n, const T& lower, const T& upper) {
  return std::max(lower, std::min(n, upper));
}
#endif // UTIL_H
