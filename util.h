#ifndef UTIL_H
#define UTIL_H
#include <algorithm>
template <typename T>
T clip(const T& n, const T& lower, const T& upper) {
  return std::max(lower, std::min(n, upper));
}
template <typename T>
bool isOnlyOneBitSet(T num)
{
    return (num == (num & -num));
}
template <typename U>
bool isBitSet(unsigned int bitIndex, U value)
{
    return (value & (1UL<<bitIndex));
}
int mvToCode(double val, double inputRange);
double codeToMv(int val, double inputRange);
#endif // UTIL_H

