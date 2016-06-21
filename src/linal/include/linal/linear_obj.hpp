#pragma once

#include <linal/vec.hpp>


namespace Linal
{

/// Type of intersection, works for both planes and 2D lines.
enum class IntersectionType
{
    COINCIDE,
    PARALLEL,
    INTERSECT,
};

/// Representations of plane in 3D and line in 2D have a lot in common.
/// This class has common code of both implementations.
template <typename T>
class LinearObj
{
public:
    explicit LinearObj(const Vec<T> &norm, const T &c)
        : norm(norm)
        , c(c)
    {
    }

    Vec<T> & normVector()
    {
        return norm;
    }

    const Vec<T> & normVector() const
    {
        return norm;
    }

    T constTerm() const
    {
        return c;
    }

    /// Return arbitrary point lying on the line/plane.
    auto basepoint() const
    {
        using PNT_T = decltype(float() / T());
        auto res = Vec<PNT_T>(norm.ndim(), PNT_T(0));
        for (int i = 0; i < norm.ndim(); ++i)
            if (std::abs(norm[i]) >= epsilon())
            {
                res[i] = PNT_T(c) / norm[i];  // zero the rest of the coordinates
                return res;
            }

        assert(false);  // Should have returned at this point. Zero lines not supported.
        std::fill(res.ptr(), res.ptr() + res.ndim(), std::numeric_limits<PNT_T>::quiet_NaN());
        return res;
    }

    template <typename S>
    bool isParallelTo(const LinearObj<S> &obj) const
    {
        assert(norm.ndim() == obj.normVector().ndim());
        return norm.isParallelTo(obj.normVector());
    }

    template <typename S>
    bool isSameAs(const LinearObj<S> &obj) const
    {
        if (!isParallelTo(obj))
            return false;

        /* Generate points on both lines.
         * If vector connecting points is zero or orthogonal to normal of the line - we have identical lines.
         */
        const auto pnt1 = basepoint();
        const auto pnt2 = obj.basepoint();
        const auto connectingVector = pnt2 - pnt1;
        return connectingVector.isZero() || connectingVector.isOrthogonalTo(norm);
    }

    template <typename S>
    bool hasPoint(const Vec<S> &pnt) const
    {
        assert(norm.ndim() == pnt.ndim());
        return std::abs(pnt.dot(norm) - c) < epsilon();
    }

    template <typename S>
    IntersectionType getIntersectionType(const LinearObj<S> &obj) const
    {
        assert(norm.ndim() == obj.normVector().ndim());
        if (isSameAs(obj))
            return IntersectionType::COINCIDE;
        else if (isParallelTo(obj))
            return IntersectionType::PARALLEL;

        return IntersectionType::INTERSECT;
    }

protected:
    static constexpr double epsilon()
    {
        return 1e-5;
    }

protected:
    /// We use equations like Ax + By = c where [A, B] == norm.
    /// Generalizes for higher dimensions.
    Vec<T> norm;
    T c;
};

}