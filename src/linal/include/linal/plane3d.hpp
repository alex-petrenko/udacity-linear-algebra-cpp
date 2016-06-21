#pragma once

#include <linal/linear_equation.hpp>


namespace Linal
{

template <typename T>
class Plane3D : public LinearEquation<T>
{
public:
    explicit Plane3D(const Vec<T> &norm, const T &c)
        : LinearEquation(norm, c)
    {
        assert(norm.ndim() == 3);
    }
};

// Using alias for convenience.
using Plane3i = Plane3D<int>;
using Plane3f = Plane3D<float>;
using Plane3d = Plane3D<double>;

}