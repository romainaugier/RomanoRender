#pragma once


#include "vec3.h"
#include "maths_utils.h"


#ifndef MATRIX
#define MATRIX


struct mat44
{
    float m[16];

    mat44() : m{ 1.0f, 0.0f, 0.0f, 0.0f,
                 0.0f, 1.0f, 0.0f, 0.0f,
                 0.0f, 0.0f, 1.0f, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f } {}

    mat44(float a, float b, float c, float d,
          float e, float f, float g, float h,
          float i, float j, float k, float l,
          float m, float n, float o, float p)
        : m{ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p } {};

    const float& operator [] (int i) const { return m[i]; }
    float& operator [] (int i) { return m[i]; }
};


inline void transpose(mat44& m)
{
    mat44 temp = m;
    m[0] = temp[0]; m[1] = temp[4]; m[2] = temp[8]; m[3] = temp[12];
    m[4] = temp[1], m[5] = temp[5]; m[6] = temp[9]; m[7] = temp[13];
    m[8] = temp[2]; m[9] = temp[6]; m[10] = temp[10]; m[11] = temp[14];
    m[12] = temp[3]; m[13] = temp[7]; m[14] = temp[11]; m[15] = temp[15];
}


inline void set_translation(mat44& m, const vec3& t)
{
    m[3] = t.x;
    m[7] = t.y;
    m[11] = t.z;
}


inline void set_rotation(mat44& m, const vec3& r)
{
    /*
    // rotate x
    float sinTheta = sin(deg2rad(r.x));
    float cosTheta = cos(deg2rad(r.x));

    m[5] = cosTheta;
    m[6] = -sinTheta;
    m[8] = sinTheta;
    m[9] = cosTheta;

    
    // rotate y
    sinTheta = sin(deg2rad(r.y));
    cosTheta = cos(deg2rad(r.y));

    m[0] = cosTheta;
    m[2] = sinTheta;
    m[8] = -sinTheta;
    m[10] = cosTheta;

    // rotate z
    sinTheta = sin(deg2rad(r.z));
    cosTheta = cos(deg2rad(r.z));

    m[0] = cosTheta;
    m[1] = -sinTheta;
    m[4] = sinTheta;
    m[5] = cosTheta;
    */
}


inline void set_scale(mat44& m, const vec3& s)
{
    m[0] = s.x;
    m[5] = s.y;
    m[10] = s.z;
}


inline vec3 transform(const vec3& v, const mat44& m)
{
    return vec3(v.x * m[0] + v.y * m[1] + v.z * m[2] + m[3],
                v.x * m[4] + v.y * m[5] + v.z * m[6] + m[7],
                v.x * m[8] + v.y * m[9] + v.z * m[10] + m[11]);
}


#endif