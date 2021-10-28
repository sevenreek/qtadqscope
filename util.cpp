#include "util.h"
#include <cmath>
#include <spdlog/fmt/fmt.h>
std::vector<char> ILLEGAL_CHARACTERS = {'/', '<', '>', ':', '"', '/', '\\','|', '?', '*'};
int mvToCode(double val, double inputRange)
{
    return std::lround ( val / ( inputRange / 2 ) * std::pow(2,15) );
}

double codeToMv(int val, double inputRange)
{
    return (double)val * (inputRange/2) / std::pow(2,15);
}

std::string removeIllegalFilenameChars(const std::string &s, char replaceWith)
{
    std::string result = s;
    for(size_t i=0; i < result.length(); i++)
    {
        for(size_t chi = 0; chi < ILLEGAL_CHARACTERS.size(); chi++)
        {
            if(result[i] == ILLEGAL_CHARACTERS[chi])
            {
                result[i] = replaceWith;
                break;
            }
        }
    }
    return result;
}

std::string doubleToPrefixNotation(double value)
{
    int timesDivided = 0;
    while(value >= 1000)
    {
        value/=1000.0;
        timesDivided++;
    }
    return fmt::format("{:.2f} {}", value, UNIT_PREFIXES[timesDivided]);
}
