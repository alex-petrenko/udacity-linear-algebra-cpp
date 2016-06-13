#pragma once

#include <cstring>
#include <cassert>
#include <initializer_list>


namespace Linal
{

template<typename T> class Vec
{
public:
    Vec()
    {
    }

    template <typename INIT_T>
    Vec(std::initializer_list<INIT_T> l)
        : size(l.size())
    {
        data = new T[size];
        std::copy(l.begin(), l.end(), data);
    }

    // copy constructor
    Vec(const Vec<T> &v)
        : size(v.size)
    {
        data = new T[size];
        memcpy(data, v.data, size * sizeof(T));
    }

    // template copy constructor (allows type cast)
    template <typename INIT_T>
    Vec(const Vec<INIT_T> &v)
        : size(v.ndim())
    {
        data = new T[size];
        for (int i = 0; i < size; ++i)
            data[i] = v[i];  // INIT_T should be castable to T
    }

    // move constructor
    Vec(Vec<T> &&v)
    {
        swap(*this, v);
    }

    // copy-assignment using copy-and-swap idiom
    // if "v" is initialized with rvalue, it will be move-constructed (see above)
    Vec<T> & operator=(Vec<T> v)
    {
        swap(*this, v);
        return *this;
    }

    ~Vec()
    {
        if (data) delete[] data;
    }

    int ndim() const
    {
        return size;
    }

    bool operator==(const Vec<T> &v) const
    {
        assert(size == v.size);
        return memcmp(data, v.data, size * sizeof(T)) == 0;
    }

    bool operator!=(const Vec<T> &v) const
    {
        return !(*this == v);
    }

    /* Synonim of operator==, but allows initializer lists:
     * v == {x, y};  // does not work
     * v.equalTo({x, y});  // works
     */
    bool equalTo(const Vec<T> &v) const
    {
        return *this == v;
    }

    template<typename ELEM_T>
    bool almostEqualTo(const Vec<ELEM_T> &v, float eps = 1e-5f) const
    {
        assert(size == v.size);
        for (int i = 0; i < size; ++i)
            if (std::fabs(data[i] - v.data[i]) > eps)
                return false;
        return true;
    }

    bool almostEqualTo(const Vec<T> &v, float eps = 1e-5) const
    {
        return almostEqualTo<T>(v, eps);
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

    Vec<T> & operator+=(const Vec<T> &v)
    {
        assert(size == v.size);
        for (int i = 0; i < size; ++i)
            data[i] += v.data[i];
        return *this;
    }

    Vec<T> & operator-=(const Vec<T> &v)
    {
        assert(size == v.size);
        for (int i = 0; i < size; ++i)
            data[i] -= v.data[i];
        return *this;
    }

    template <typename SCALAR_T>
    Vec<T> & operator*=(const SCALAR_T &scalar)
    {
        for (int i = 0; i < size; ++i)
            data[i] *= scalar;
        return *this;
    }

public:  // friends defined inside class body are also inline
    friend Vec<T> operator+(Vec<T> v, const Vec<T> &w)
    {
        v += w;  // reuse compound assignment
        return v;  // return the result by value (uses move constructor)
    }

    friend Vec<T> operator-(Vec<T> v, const Vec<T> &w)
    {
        v -= w;  // reuse compound assignment
        return v;  // return the result by value (uses move constructor)
    }

    template <typename SCALAR_T>
    friend Vec<T> operator*(Vec<T> v, const SCALAR_T &scalar)
    {
        v *= scalar;
        return v;
    }

    template <typename SCALAR_T>
    friend Vec<T> operator*(const SCALAR_T &scalar, Vec<T> v)
    {
        return v * scalar;
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

private:
    T *data = nullptr;
    int size = 0;
};

}