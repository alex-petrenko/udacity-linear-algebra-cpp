#pragma once

#include <cstring>
#include <cassert>
#include <numeric>
#include <initializer_list>

#include <linal/util.hpp>
#include <linal/sfinae.hpp>


// TODO: use universal init in ctors


namespace Linal
{

template<typename T>
class Vec
{
public:
    typedef T ElementType;

public:
    Vec() = default;

    /// Explicit constructor does not allow conversion from Vec to int (which does not make sense).
    explicit Vec(int size)
        : size(size)
        , data(new T[size])
    {
    }

    /// Fill vector with same values.
    explicit Vec(int size, const T &value)
        : Vec(size)
    {
        std::fill(data, data + size, value);
    }

    template <typename INIT_T>
    Vec(std::initializer_list<INIT_T> l)
        : Vec(l.size())
    {
        /* Currently there's no way to move from initializer_list, so we're copying.
         * http://stackoverflow.com/questions/8193102/initializer-list-and-move-semantics
         */
        std::copy(l.begin(), l.end(), data);
    }

    /// Copy constructor.
    Vec(const Vec<T> &v)
        : Vec(v.size)
    {
        memcpy(data, v.data, size * sizeof(T));
    }

    /// Template copy constructor (allows type cast).
    template <typename INIT_T>
    Vec(const Vec<INIT_T> &v)
        : Vec(v.ndim())
    {
        for (int i = 0; i < size; ++i)
            data[i] = v[i];  // INIT_T should be castable to T
    }

    /// Move constructor.
    Vec(Vec<T> &&v)
    {
        swap(*this, v);
    }

    /// Copy-assignment using copy-and-swap idiom.
    /// If "v" is initialized with rvalue, it will be move-constructed (see above).
    /// Due to this function there's no need to define separate move assignment operator.
    Vec<T> & operator=(Vec<T> v)
    {
        swap(*this, v);
        return *this;
    }

    ~Vec()
    {
        if (data) delete[] data;
    }

    /// Number of dimensions.
    int ndim() const
    {
        return size;
    }

    /// Access raw vector data. Can be used instead of iterators.
    T * ptr()
    {
        return data;
    }

    /// Use instead of const iterator.
    const T * ptr() const
    {
        return data;
    }

    T & operator[](int idx)
    {
        assert(idx >= 0 && idx < size);
        return data[idx];
    }

    const T & operator[](int idx) const
    {
        assert(idx >= 0 && idx < size);
        return data[idx];
    }

    bool operator==(const Vec<T> &v) const
    {
        assert(size == v.size);
        return memcmp(data, v.data, size * sizeof(T)) == 0;
    }

    /// Allows comparison of vectors of different type, as long as their elements are comparable.
    template <typename S>
    bool operator==(const Vec<S> &v) const
    {
        assert(size == v.ndim());
        return std::equal(data, data + size, v.ptr());
    }

    template <typename S>
    bool operator!=(const Vec<S> &v) const
    {
        return !(*this == v);  // will call optimized Vec<T> version if T and S are the same
    }

    template <typename S>
    Vec<T> & operator+=(const Vec<S> &v)
    {
        assert(size == v.ndim());
        for (int i = 0; i < size; ++i)
            data[i] += v[i];
        return *this;
    }

    Vec<T> & operator+=(const Vec<T> &v)
    {
        return operator+=<T>(v);
    }

    template <typename S>
    Vec<T> & operator-=(const Vec<S> &v)
    {
        assert(size == v.ndim());
        for (int i = 0; i < size; ++i)
            data[i] -= v[i];
        return *this;
    }

    Vec<T> & operator-=(const Vec<T> &v)
    {
        return operator-=<T>(v);
    }

    template <typename SCALAR_T>
    Vec<T> & operator*=(const SCALAR_T &scalar)
    {
        for (int i = 0; i < size; ++i)
            data[i] *= scalar;
        return *this;
    }
    
    template <typename SCALAR_T>
    Vec<T> & operator/=(const SCALAR_T &scalar)
    {
        return *this *= T(1) / scalar;
    }

    bool isNaN() const
    {
        for (int i = 0; i < size; ++i)
            if (isnan(double(data[i])))
                return true;
        return false;
    }

    bool isZero() const
    {
        for (int i = 0; i < size; ++i)
            if (std::abs(data[i]) >= epsilon())
                return false;
        return true;
    }

    /// Synonim of operator==, but allows initializer lists:
    /// v == {x, y};  // does not work
    /// v.equalTo({x, y});  // works
    bool equalTo(const Vec<T> &v) const
    {
        return *this == v;
    }

    /// This version does not allow initializer lists, because C++ cannot deduce
    /// type 'S' from the elements of initializer list. The previous version will do.
    /// See http://en.cppreference.com/w/cpp/language/template_argument_deduction (non-deduced contexts).
    /// This means that in this code:
    /// Vec<double> v{1.0, 2.0}; v.equalTo({int(1), int(2)});
    /// the temporary Vec<double> will be created for comparison. But in this case:
    /// Vec<int> vi{1, 2}; v.equalTo(vi);
    /// Vec<int> and Vec<double> will be compared directly element-by-element. See tests for reference.
    template <typename S>
    bool equalTo(const Vec<S> &v) const
    {
        return *this == v;
    }

    template <typename S>
    bool almostEqualTo(const Vec<S> &v) const
    {
        assert(size == v.ndim());
        for (int i = 0; i < size; ++i)
            if (std::abs(data[i] - v[i]) > epsilon())
                return false;
        return true;
    }

    /// This version is mostly needed to allow initializer lists as arguments.
    bool almostEqualTo(const Vec<T> &v) const
    {
        return almostEqualTo<T>(v);  // reuse more generic template
    }

    /// Sometimes it is not required to take square root of norm, makes sense to keep this as separate method.
    T normSquared() const
    {
        T sum = T();
        for (int i = 0; i < size; ++i)
            sum += data[i] * data[i];
        return sum;
    }
    
    auto norm() const
    {
        // auto will also work for Vec<int>, while the simple T norm() wouldn't work correctly.
        return sqrt(normSquared());
    }

    /// Do not use for Vec<int>, this function will work incorrectly. Use "normalized" instead.
    void normalize()
    {
        static_assert(std::is_floating_point_v<T>, "Normalize does not work for integral types");
        const auto normValue = norm();  // cannot normalize zero vector
        if (normValue)
            *this /= normValue;  // won't work for Vec<int>
        else
            std::fill(data, data + size, std::numeric_limits<T>::quiet_NaN());  // can't normalize zero vector
    }
    
    auto normalized() const
    {
        const auto normValue = norm();
        using RESULT_T = decltype(*this / normValue);
        if (normValue)
            return *this / normValue;
        else
            return RESULT_T(size, std::numeric_limits<RESULT_T::ElementType>::quiet_NaN());
    }

    template <typename S>
    auto dot(const Vec<S> &v) const
    {
        assert(size == v.ndim());
        return std::inner_product(ptr(), ptr() + size, v.ptr(), decltype(T() * S())(0));  // did you know that STL has such function?
    }

    /// Inner product. With this method you can write v.dot({x, y}), which you can't do with operator*.
    auto dot(const Vec<T> &v) const
    {
        return dot<T>(v);
    }

    template <typename S>
    auto cross(const Vec<S> &v) const
    {
        assert(size >= 2 && size <= 3);
        assert(v.ndim() >= 2 && v.ndim() <= 3);
        const auto x1 = data[0], y1 = data[1], z1 = size >= 3 ? data[2] : T(0);
        const auto x2 = v[0], y2 = v[1], z2 = v.ndim() >= 3 ? v[2] : S(0);
        return Vec<decltype(T() * S())>{(y1 * z2 - z1 * y2), -(x1 * z2 - z1 * x2), (x1 * y2 - y1 * x2)};
    }

    auto cross(const Vec<T> &v) const
    {
        return cross<T>(v);
    }

    template <typename S>
    auto angleToRad(const Vec<S> &v) const
    {
        const auto normProd = sqrt(normSquared() * v.normSquared());
        if (normProd)
            return acos(v.dot(*this) / normProd);
        else
            return std::numeric_limits<decltype(normProd)>::quiet_NaN();
    }

    template <typename S>
    auto angleToDegrees(const Vec<S> &v) const
    {
        return radToDegrees(angleToRad(v));
    }

    template <typename S>
    bool isParallelTo(const Vec<S> &v) const
    {
        assert(size == v.ndim());
        const auto normProd = sqrt(normSquared() * v.normSquared());
        if (normProd)
        {
            // not the fastest way, I know
            const auto cosAngle = (*this * v) / normProd;
            const auto diff = std::abs(cosAngle) - decltype(normProd)(1);
            return std::abs(diff) <= epsilon();
        }
        else
            return true;
    }

    template <typename S>
    bool isOrthogonalTo(const Vec<S> &v) const
    {
        const auto innerProd = *this * v;
        return std::abs(innerProd) <= epsilon();
    }

    template <typename S>
    auto componentParallelTo(const Vec<S> &v) const
    {
        const auto vn = v.normalized();
        const auto magnitude = *this * vn;  // scalar product of *this and unit vector pointing in direction of v
        return magnitude * vn;
    }

    template <typename S>
    auto componentOrthogonalTo(const Vec<S> &v) const
    {
        const auto parallelComponent = componentParallelTo(v);
        return *this - parallelComponent;
    }

public:  // friends defined inside class body are also inline and can be found through ADL

    /// By-element vector addition.
    friend Vec<T> operator+(Vec<T> v, const Vec<T> &w)
    {
        v += w;  // reuse compound assignment
        return v;  // return the result by value (uses move constructor)
    }

    /// By-element vector addition.
    /// Allows addition of two vectors of different types and deduces the result type accordingly.
    /// E. g. Vec<int> + Vec<double> produces Vec<double>, and Vec<float> + Vec<double> produces Vec<double>.
    template <typename S>
    friend auto operator+(const Vec<T> &v, const Vec<S> &w)
    {
        return v.additionWithMultiplier<S, 1>(w);
    }

    friend Vec<T> operator-(Vec<T> v, const Vec<T> &w)
    {
        v -= w;  // reuse compound assignment
        return v;  // return the result by value (uses move constructor)
    }

    template <typename S>
    friend auto operator-(const Vec<T> &v, const Vec<S> &w)
    {
        return v.additionWithMultiplier<S, -1>(w);
    }

    /// Multiplication by scalar.
    /// Disable this overload if SCALAR_T is Vec to avoid overload ambiguity. Use SFINAE trick for that.
    template <typename SCALAR_T, typename = std::enable_if_t<!IsSpecializationOfV<Vec, SCALAR_T>>>
    friend auto operator*(const Vec<T> &v, const SCALAR_T &scalar)
    {
        using RESULT_T = decltype(T(1) * scalar);  // deducing type for result
        Vec<RESULT_T> newVec(v.size);
        for (int i = 0; i < v.size; ++i)
            newVec[i] = v[i] * scalar;

        return newVec;
    }

    template <typename SCALAR_T, typename = std::enable_if_t<!IsSpecializationOfV<Vec, SCALAR_T>>>
    friend auto operator*(const SCALAR_T &scalar, Vec<T> v)
    {
        return v * scalar;
    }

    template <typename SCALAR_T>
    friend auto operator/(const Vec<T> &v, const SCALAR_T &scalar)
    {
        return v * (SCALAR_T(1) / scalar);
    }

    /// v * w is a synonim for v.dot(w).
    template <typename S>
    friend auto operator*(const Vec<T> &v, const Vec<S> &w)
    {
        return v.dot<S>(w);
    }

    friend T operator*(const Vec<T> &v, const Vec<T> &w)
    {
        return v.dot(w);
    }

    friend std::ostream & operator<<(std::ostream &stream, const Vec<T> &v)
    {
        stream << "[";
        for (int i = 0; i < v.size; ++i)
        {
            if (i) stream << ", ";
            stream << v.data[i];
        }
        stream << "]";
        return stream;
    }

    friend void swap(Vec<T> &v, Vec<T> &w)
    {
        using std::swap;  // proper use of ADL (argument dependent lookup)
        swap(v.data, w.data);  // if T implements swap it will be selected instead of std::swap
        swap(v.size, w.size);
    }

    template <typename S>
    friend auto parallelogramArea(const Vec<T> &v, const Vec<S> &w)
    {
        return v.cross(w).norm();
    }

    template <typename S>
    friend auto triangleArea(const Vec<T> &v, const Vec<S> &w)
    {
        const auto parea = parallelogramArea(v, w);
        return parea * 0.5f;
    }

private:
    /// This is okay for educational purposes, but in real library it would be really bad design.
    /// If I ever happen to use this code in production, I should pass eps as parameter to all functions
    /// where it is needed (or use another mechanism).
    static constexpr double epsilon()
    {
        return 1e-5;
    }

    /// Common code of operator+ and operator-.
    /// This is an implementation detail, that's why it is a hidden private member.
    template <typename S, char MULT>
    auto additionWithMultiplier(const Vec<S> &w) const
    {
        using RESULT_T = decltype(T() + S());
        Vec<RESULT_T> res(size);
        for (int i = 0; i < size; ++i)
            res[i] = data[i] + MULT * w[i];
        return res;
    }

private:
    int size = 0;
    T *data = nullptr;
};


// Using alias for convenience.
using Veci = Vec<int>;
using Vecs = Vec<short>;
using Vecf = Vec<float>;
using Vecd = Vec<double>;

}