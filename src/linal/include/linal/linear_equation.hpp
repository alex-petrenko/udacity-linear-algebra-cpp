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
class LinearEquation
{
public:
    /// Force generation of default ctor. Required for stl containers.
    LinearEquation() = default;

    /// Vector passed by value guarantees that no copies will be made if an rvalue reference is passed.
    LinearEquation(Vec<T> norm, const T &c)
        : norm(std::move(norm))
        , c{ c }
    {
    }

    /// Copy constructor.
    LinearEquation(const LinearEquation<T> &eq)
        : norm(eq.norm)
        , c{ eq.c }
    {
    }

    /// Move constructor.
    LinearEquation(LinearEquation<T> &&eq)
    {
        swap(*this, eq);
    }

    /// Copy-assignment using copy-and-swap idiom.
    LinearEquation<T> & operator=(LinearEquation<T> eq)
    {
        swap(*this, eq);
        return *this;
    }

    template <typename S>
    bool operator==(const LinearEquation<S> &eq) const
    {
        assert(norm.ndim() == eq.normVector().ndim());
        return norm == eq.normVector() && c == eq.constTerm();
    }

    template <typename S>
    bool operator!=(const LinearEquation<S> &eq) const
    {
        return !(*this == eq);
    }

    template <typename SCALAR_T>
    LinearEquation<T> & operator*=(const SCALAR_T &scalar)
    {
        norm *= scalar, c *= scalar;
        return *this;
    }

    /// See Vec<T> comments to understand why two versions of this function are required.
    bool equalTo(const LinearEquation<T> &eq) const
    {
        return *this == eq;
    }

    template <typename S>
    bool equalTo(const LinearEquation<S> &eq) const
    {
        return *this == eq;
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

    int ndim() const
    {
        return norm.ndim();
    }

    int firstNonZeroIndex() const
    {
        for (int i = 0; i < norm.ndim(); ++i)
            if (std::abs(norm[i]) > epsilon())
                return i;
        return -1;
    }

    template <typename S, typename SCALAR_T>
    void add(const LinearEquation<S> &eq, const SCALAR_T &multiplier)
    {
        assert(eq.normVector().ndim() == norm.ndim());
        for (int i = 0; i < norm.ndim(); ++i)
            norm[i] += eq.normVector()[i] * multiplier;
        c += eq.constTerm() * multiplier;
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
    bool isParallelTo(const LinearEquation<S> &obj) const
    {
        assert(norm.ndim() == obj.normVector().ndim());
        return norm.isParallelTo(obj.normVector());
    }

    template <typename S>
    bool isSameAs(const LinearEquation<S> &obj) const
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
    IntersectionType getIntersectionType(const LinearEquation<S> &obj) const
    {
        assert(norm.ndim() == obj.normVector().ndim());
        if (isSameAs(obj))
            return IntersectionType::COINCIDE;
        else if (isParallelTo(obj))
            return IntersectionType::PARALLEL;

        return IntersectionType::INTERSECT;
    }

public:  // friends defined inside class body are also inline and can be found through ADL
    friend void swap(LinearEquation<T> &eq1, LinearEquation<T> &eq2)
    {
        using std::swap;  // proper use of ADL (argument dependent lookup)
        swap(eq1.norm, eq2.norm);  // should call cheap Vec's swap
        swap(eq1.c, eq2.c);
    }

    friend std::ostream & operator<<(std::ostream &stream, const LinearEquation<T> &eq)
    {
        stream << eq.norm << " = " << eq.c;
        return stream;
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