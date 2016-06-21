#pragma once

#include <linal/linear_equation.hpp>


namespace Linal
{

template <typename T>
class LineIntersection2D
{
public:
    explicit LineIntersection2D(IntersectionType type, const Vec<T> *pnt = nullptr)
        : type(type)
        , pnt(pnt)
    {
        assert((type == IntersectionType::INTERSECT && pnt) || (type != IntersectionType::INTERSECT && !pnt));
    }

    ~LineIntersection2D()
    {
        assert((type == IntersectionType::INTERSECT && pnt) || (type != IntersectionType::INTERSECT && !pnt));
        if (pnt)
            delete pnt, pnt = nullptr;
    }

public:
    friend std::ostream & operator<<(std::ostream &stream, const LineIntersection2D<T> &intersection)
    {
        stream << "Type: " << std::underlying_type_t<IntersectionType>(intersection.type);
        if (intersection.pnt)
            stream << " pnt: " << *intersection.pnt;
        return stream;
    }

public:
    IntersectionType type;
    const Vec<T> *pnt;
};

template <typename T>
class Line2D : public LinearEquation<T>
{
public:
    explicit Line2D(const Vec<T> &norm, const T &c)
        : LinearEquation(norm, c)
    {
        assert(norm.ndim() == 2);
    }

    template <typename S>
    auto getIntersectionWith(const Line2D<S> &line) const
    {
        using PNT_T = decltype(float() / T());
        using INTERSECTION_T = LineIntersection2D<PNT_T>;

        const IntersectionType type = getIntersectionType(line);
        if (type != IntersectionType::INTERSECT)
            return INTERSECTION_T(type);

        // find the actual intersection point
        const T A = norm[0], B = norm[1], k1 = c;
        const S C = line.normVector()[0], D = line.normVector()[1], k2 = line.constTerm();

        /* These formulas come from system of equations
        * 1) Ax + By = k1
        * 2) Cx + Dy = k2
        */
        const auto denominator = 1.0f / (A * D - B * C);
        assert(std::abs(denominator) >= epsilon());
        PNT_T x, y;
        x = (D * k1 - B * k2) * denominator;
        y = (-C * k1 + A * k2) * denominator;
        return INTERSECTION_T(IntersectionType::INTERSECT, new Vec<PNT_T>{ x, y });
    }
};

// Using alias for convenience.
using Line2i = Line2D<int>;
using Line2f = Line2D<float>;
using Line2d = Line2D<double>;

}