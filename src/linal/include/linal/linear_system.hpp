#pragma once

#include <linal/linear_equation.hpp>


namespace Linal
{

template <typename T>
class LinearSystem
{
public:
    LinearSystem(std::initializer_list<LinearEquation<T>> l)
        : eqs(l.size())
    {
        /* Currently there's no way to move from initializer_list, so we're actually copying.
        * http://stackoverflow.com/questions/8193102/initializer-list-and-move-semantics
        */
        std::move(l.begin(), l.end(), eqs.begin());
    }

    const LinearEquation<T> & operator[](int idx) const
    {
        return eqs[idx];
    }

    LinearEquation<T> & operator[](int idx)
    {
        return eqs[idx];
    }

private:
    std::vector<LinearEquation<T>> eqs;
};

}