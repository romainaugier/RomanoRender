#pragma once

#include <math.h>

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                3-D VECTOR                                */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

template<typename T>
struct tvec3
{
    T e[3];

    tvec3() : e{ 0,0,0 } {}
    tvec3(const T& s) : e{ s,s,s } {}
    tvec3(const T& e0, const T& e1, const T& e2) : e{ e0,e1,e2 } {}
    tvec3(const tvec3& v) : e{ v[0], v[1], v[2] } {}

    T& operator[](int i) { return e[i]; }
    const T& operator[](int i) const { return e[i]; }

    tvec3<T>& operator=(const tvec3<T>& v)
    {
        e[0] = v[0];
        e[1] = v[1];
        e[2] = v[2];

        return *this;
    }

    tvec3<T>& operator+=(const tvec3<T>& v)
    {
        e[0] += v[0];
        e[1] += v[1];
        e[2] += v[2];

        return *this;
    }

    tvec3<T>& operator-=(const tvec3<T>& v)
    {
        e[0] -= v[0];
        e[1] -= v[1];
        e[2] -= v[2];

        return *this;
    }

    tvec3<T>& operator*=(const tvec3<T>& v)
    {
        e[0] *= v[0];
        e[1] *= v[1];
        e[2] *= v[2];

        return *this;
    }

    tvec3<T>& operator/=(const tvec3<T>& v)
    {
        e[0] /= v[0];
        e[1] /= v[1];
        e[2] /= v[2];

        return *this;
    }

    tvec3<T>& operator*=(const T& s)
    {
        e[0] *= s;
        e[1] *= s;
        e[2] *= s;

        return *this;
    }

    tvec3<T>& operator/=(const T& s)
    {
        e[0] /= s;
        e[1] /= s;
        e[2] /= s;

        return *this;
    }
};

/*------------------------ componentwise operations ------------------------*/

template<typename T>
tvec3<T> operator+(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return tvec3<T>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]);
}

template<typename T>
tvec3<T> operator-(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return tvec3<T>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]);
}

template<typename T>
tvec3<T> operator*(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return tvec3<T>(lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2]);
}

template<typename T>
tvec3<T> operator/(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return tvec3<T>(lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2]);
}

/*---------------------- scalar-vector/vector-scalar -----------------------*/

template<typename T>
tvec3<T> operator*(const tvec3<T>& lhs, const T& rhs)
{
    return tvec3<T>(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs);
}

template<typename T>
tvec3<T> operator*(const T& lhs, const tvec3<T>& rhs)
{
    return tvec3<T>(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2]);
}

template<typename T>
tvec3<T> operator/(const tvec3<T>& lhs, const T& rhs)
{
    return tvec3<T>(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs);
}

template<typename T>
tvec3<T> operator/(const T& lhs, const tvec3<T>& rhs)
{
    return tvec3<T>(lhs / rhs[0], lhs / rhs[1], lhs / rhs[2]);
}

/*---------------------------- misc operations -----------------------------*/

template<typename T>
T dot(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
}

template<typename T>
tvec3<T> cross(const tvec3<T>& lhs, const tvec3<T>& rhs)
{
    return tvec3<T>(lhs[1] * rhs[2] - lhs[2] * rhs[1], lhs[2] * rhs[0] - lhs[0] * rhs[2], lhs[0] * rhs[1] - lhs[1] * rhs[0]);
}

template<typename T>
T length_squared(const tvec3<T>& v)
{
    return dot(v, v);
}

template<typename T>
T length(const tvec3<T>& v)
{
    return (T)sqrt(length_squared(v));
}

template<typename T>
T normalize(const tvec3<T>& v)
{
    return v / length(v);
}

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                3x3 MATRIX                                */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

template<typename T>
struct tmat3
{
    tvec3<tvec3<T>> e;

    tmat3() {}
    tmat3(const T& s) : e(s) {}
    tmat3(const tvec3<T>& e0, const tvec3<T>& e1, const tvec3<T>& e2) : e(e0, e1, e2) {}
    tmat3(const tmat3<T>& m) : e(m[0], m[1], m[2]) {}

    tvec3<T>& operator[](int i) { return e[i]; }
    const tvec3<T>& operator[](int i) const { return e[i]; }

    T& operator()(int i, int j) { return e[i][j]; }
    const T& operator()(int i, int j) { return e[i][j]; }

};

/*------------------------ componentwise operations ------------------------*/

template<typename T>
tmat3<T> operator+(const tmat3<T>& lhs, const tmat3<T>& rhs)
{
    return tmat3<T>(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]);
}

template<typename T>
tmat3<T> operator-(const tmat3<T>& lhs, const tmat3<T>& rhs)
{
    return tmat3<T>(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]);
}

/*---------------------- scalar-vector/vector-scalar -----------------------*/

template<typename T>
tmat3<T> operator*(const tmat3<T>& lhs, const T& rhs)
{
    return tmat3<T>(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs);
}

template<typename T>
tmat3<T> operator*(const T& lhs, const tmat3<T>& rhs)
{
    return tmat3<T>(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2]);
}

template<typename T>
tmat3<T> operator/(const tmat3<T>& lhs, const T& rhs)
{
    return tmat3<T>(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs);
}

template<typename T>
tmat3<T> operator/(const T& lhs, const tmat3<T>& rhs)
{
    return tmat3<T>(lhs / rhs[0], lhs / rhs[1], lhs / rhs[2]);
}

/*---------------------------- misc operations -----------------------------*/

template<typename T>
tmat3<T> transpose(const tmat3<T>& m)
{
    return tmat3<T>(tvec3<T>(m(0, 0), m(1, 0), m(2, 0)),
        tvec3<T>(m(0, 1), m(1, 1), m(2, 1)),
        tvec3<T>(m(0, 2), m(1, 2), m(2, 2)));
}

template<typename T>
tvec3<T> operator*(const tmat3<T>& lhs, const tvec3<T>& rhs)
{
    return tvec3<T>(dot(lhs[0], rhs), dot(lhs[1], rhs), dot(lhs[2], rhs));
}

template<typename T>
tvec3<T> operator*(const tvec3<T>& lhs, const tmat3<T>& rhs)
{
    return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * lhs[2];
}

template<typename T>
tmat3<T> operator*(const tmat3<T>& lhs, const tmat3<T>& rhs)
{
    return tmat3<T>(lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs);
}

template<typename T>
tmat3<T> cofactor(const tmat3<T>& m)
{
    return tmat3<T>(cross(m[1], m[2]), cross(m[2], m[0]), cross(m[0], m[1]));
}

template<typename T>
tmat3<T> determinant(const tmat3<T>& m)
{
    return dot(m[0], cross(m[1], m[2]));
}

template<typename T>
tmat3<T> inverse_transpose(const tmat3<T>& m)
{
    tvec3<T> e12 = cross(m[1], m[2]);
    T det = dot(m[0], e12);
    return tmat3<T>(e12 / det, cross(m[2], m[0]) / det, cross(m[0], m[1]) / det);
}

/*--------------------------------------------------------------------------*/
