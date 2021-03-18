#pragma once


#include "vec3.h"
#include "maths_utils.h"


#ifndef MATRIX
#define MATRIX


struct mat44
{
    float m[4][4];

    mat44() : m{{ 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f, 1.0f }} {}

    mat44(float a, float b, float c, float d,
          float e, float f, float g, float h,
          float i, float j, float k, float l,
          float m, float n, float o, float p)
        : m{ { a, b, c, d }, { e, f, g, h }, { i, j, k, l }, { m, n, o, p } } {};

    const float* operator [] (int i) const { return m[i]; }
    float* operator [] (int i) { return m[i]; }
};


inline mat44 operator*(mat44& m1, mat44& m2)
{
    mat44 temp = mat44();

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp[i][j] = m1[i][0] * m2[0][j] +
                         m1[i][1] * m2[1][j] +
                         m1[i][2] * m2[2][j] +
                         m1[i][3] * m2[3][j];
        }
    }

    return temp;
}


inline void transpose(mat44& m)
{
    m = mat44(m[0][0], m[1][0], m[2][0], m[3][0],
              m[0][1], m[1][1], m[2][1], m[3][1],
              m[0][2], m[1][2], m[2][2], m[3][2],
              m[0][3], m[1][3], m[2][3], m[3][3]);
}


inline void set_translation(mat44& m, const vec3& t)
{
    m[0][3] = t.x;
    m[1][3] = t.y;
    m[2][3] = t.z;
}


inline void set_rotation(mat44& m, const vec3& r)
{
    
    // rotate x
    mat44 rx = mat44();
    float sinTheta = sin(deg2rad(r.x));
    float cosTheta = cos(deg2rad(r.x));

    rx[1][1] = cosTheta;
    rx[1][2] = -sinTheta;
    rx[2][1] = sinTheta;
    rx[2][2] = cosTheta;

    
    // rotate y
    mat44 ry = mat44();
    sinTheta = sin(deg2rad(r.y));
    cosTheta = cos(deg2rad(r.y));

    ry[0][0] = cosTheta;
    ry[0][2] = sinTheta;
    ry[2][0] = -sinTheta;
    ry[2][2] = cosTheta;

    // rotate z
    mat44 rz = mat44();
    sinTheta = sin(deg2rad(r.z));
    cosTheta = cos(deg2rad(r.z));

    rz[0][0] = cosTheta;
    rz[0][1] = -sinTheta;
    rz[1][0] = sinTheta;
    rz[1][1] = cosTheta;
    
    m = m * (rx * ry * rz);
}


inline void set_scale(mat44& m, const vec3& s)
{
    m[0][0] = s.x;
    m[1][1] = s.y;
    m[2][2] = s.z;
}


inline vec3 transform(const vec3& v, const mat44& m)
{
    float w = v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + m[3][3];

    return vec3((v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + m[0][3]),
                (v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + m[1][3]),
                (v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + m[2][3]));
}

inline vec3 transform_dir(const vec3& v, const mat44& m)
{
    return vec3(v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2],
                v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2],
                v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2]);
}


#endif