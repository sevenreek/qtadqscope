#ifndef UTIL_H
#define UTIL_H
#include <algorithm>
#include <string>
#include <vector>
#include <QJsonObject>
const char UNIT_PREFIXES[7]  = {' ', 'k', 'M', 'G', 'P', 'Y', 'Z'};
class JSONSerializable {
public:
    virtual QJsonObject toJSON() = 0;
};
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
extern std::vector<char> ILLEGAL_CHARACTERS;
std::string removeIllegalFilenameChars(const std::string &s, char replaceWith);
std::string doubleToPrefixNotation(double value);
#endif // UTIL_H

