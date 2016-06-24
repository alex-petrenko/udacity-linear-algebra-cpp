#pragma once

#include <linal/linear_equation.hpp>


namespace Linal
{

template <typename T>
class Plane3D : public LinearEquation<T>
{
public:
    /// The combination of rvalue reference and std::forward is called "perfect forwarding".
    /// No matter what kind of reference was passed to Plane3D ctor, the type will be unchanged in
    /// LinearEquation ctor.
    explicit Plane3D(const Vec<T> &&norm, const T &c)
        : LinearEquation(std::forward<const Vec<T>>(norm), c)
    {
        assert(norm.ndim() == 3);
    }
};

// Using alias for convenience.
using Plane3i = Plane3D<int>;
using Plane3f = Plane3D<float>;
using Plane3d = Plane3D<double>;

}