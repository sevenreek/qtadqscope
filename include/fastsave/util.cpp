#include <algorithm>
namespace util {
    float clamp(float n, float lower, float upper) {
    return std::max(lower, std::min(n, upper));
    }
}
