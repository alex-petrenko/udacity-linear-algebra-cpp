#pragma once

#include <linal/vec.hpp>


namespace Linal
{

enum class IntersectionType
{
    COINCIDE,
    PARALLEL,
    INTERSECT,
};

template <typename T>
class LineIntersection  // 2D?
{
public:
    explicit LineIntersection(IntersectionType type, const Vec<T> *pnt = nullptr)
        : type(type)
        , pnt(pnt)
    {
        assert((type == IntersectionType::INTERSECT && pnt) || (type != IntersectionType::INTERSECT && !pnt));
    }

    ~LineIntersection()
    {
        assert((type == IntersectionType::INTERSECT && pnt) || (type != IntersectionType::INTERSECT && !pnt));
        if (pnt)
            delete pnt, pnt = nullptr;
    }

public:
    friend std::ostream & operator<<(std::ostream &stream, const LineIntersection<T> &intersection)
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
class Line
{
public:
    explicit Line(const Vec<T> &normVec, const T &c)
        : normVec(normVec)
        , c(c)
    {
    }

    Vec<T> & normVector()
    {
        return normVec;
    }

    const Vec<T> & normVector() const
    {
        return normVec;
    }

    T constTerm() const
    {
        return c;
    }

    /// Return arbitrary point lying on the line.
    auto pointOnLine() const
    {
        assert(normVector().ndim() == 2);

        const T a = normVector()[0], b = normVector()[1];
        assert(std::abs(a) >= epsilon() || std::abs(b) >= epsilon());  // don't support zero lines

        using PNT_T = decltype(float() / T());
        PNT_T x, y;
        if (std::abs(a) >= epsilon())
            y = 0.0f, x = PNT_T(c) / a;
        else
            x = 0.0f, y = PNT_T(c) / b;

        return Vec<PNT_T>{x, y};
    }

    template <typename S>
    bool isParallelTo(const Line<S> &line) const
    {
        assert(normVector().ndim() == line.normVector().ndim());
        assert(normVector().ndim() == 2);

        return normVector().isParallelTo(line.normVector());
    }

    template <typename S>
    bool isSameAs(const Line<S> &line) const
    {
        if (!isParallelTo(line))
            return false;

        /* Generate points on both lines.
         * If vector connecting points is zero or orthogonal to normal of the line - we have identical lines.
         */
        const auto pnt1 = pointOnLine();
        const auto pnt2 = line.pointOnLine();
        const auto vecConnectingLines = pnt2 - pnt1;
        return vecConnectingLines.isZero() || vecConnectingLines.isOrthogonalTo(normVector());
    }

public:  // friends defined inside class body are also inline and can be found through ADL
    template <typename S>
    friend bool pointLiesOnLine(const Vec<S> &pnt, const Line<T> &line)
    {
        assert(pnt.ndim() == 2 && line.normVector().ndim() == 2);
        const auto x = pnt[0], y = pnt[1];
        const auto a = line.normVector()[0], b = line.normVector()[1], c = line.constTerm();
        return std::abs((a * x + b * y) - c) < epsilon();
    }

    template <typename S>
    friend auto intersection(const Line<T> &line1, const Line<S> &line2)
    {
        using PNT_T = decltype(float() / T());
        using INTERSECTION_T = LineIntersection<PNT_T>;

        if (line1.isSameAs(line2))
            return INTERSECTION_T(IntersectionType::COINCIDE);
        else if (line1.isParallelTo(line2))
            return INTERSECTION_T(IntersectionType::PARALLEL);
        else
        {
            const T A = line1.normVector()[0], B = line1.normVector()[1], k1 = line1.constTerm();
            const S C = line2.normVector()[0], D = line2.normVector()[1], k2 = line2.constTerm();

            /* These formulas come from system of equations
             * 1) Ax + By = k1
             * 2) Cx + Dy = k2
             */
            const auto denominator = 1.0f / (A * D - B * C);
            assert(std::abs(denominator) >= line1.epsilon());
            PNT_T x, y;
            x = (D * k1 - B * k2) * denominator;
            y = (-C * k1 + A * k2) * denominator;
            return INTERSECTION_T(IntersectionType::INTERSECT, new Vec<PNT_T>{ x, y });
        }
    }

private:
    static constexpr double epsilon()
    {
        return 1e-5;
    }

private:
    /// We use equation Ax + By = C where [A, B] == normVector and C == constTerm.
    /// Generalizes for higher dimensions (Ax + By + Cz = D).
    Vec<T> normVec;
    T c;
};

// Using alias for convenience.
using Linei = Line<int>;
using Linef = Line<float>;
using Lined = Line<double>;


}