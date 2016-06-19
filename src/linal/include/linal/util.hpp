#pragma once

#include <cmath>

namespace Linal
{

template<typename T>
auto radToDegrees(T rad)
{
    return radToDegrees(rad, std::is_floating_point<T>());
}

/// Convert radians to degrees. This is an overload for float and double.
/// Will keep calculations in float/double accordingly.
template<typename T>
auto radToDegrees(T rad, std::true_type)
{
    static constexpr T coeff = T(180.0) / T(M_PI);
    return rad * coeff;
}

/// Overload for non floating point types. Convert everything to double.
template<typename T>
auto radToDegrees(T rad, std::false_type)
{
    static constexpr double coeff = 180.0 / M_PI;
    return rad * coeff;
}

}