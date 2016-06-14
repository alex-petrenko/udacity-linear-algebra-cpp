#pragma once

#include <cmath>


double radToDegrees(double rad)
{
    return rad * (180.0 / M_PI);
}

float radToDegrees(float rad)
{
    return rad * (180.0f / float(M_PI));
}