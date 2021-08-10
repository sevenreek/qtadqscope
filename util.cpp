#include "util.h"
#include <cmath>

int mvToCode(double val, double inputRange)
{
    return std::round ( val / ( inputRange / 2 ) * std::pow(2,15) );
}

double codeToMv(int val, double inputRange)
{
    return (double)val * (inputRange/2) / std::pow(2,15);
}
