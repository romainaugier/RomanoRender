#pragma once
#include <vector>
#include <random>
#include "GLFW/glfw3.h"
#include "matrix.h"

#define _CRT_SECURE_NO_WARNINGS

#define INV_PI = 0.31830988618379067154

#ifndef UTILS
#define UTILS


struct Vertex { float x, y, z, r; };
struct Triangle { int v0, v1, v2; };

#include <embree3/rtcore.h>

void errorFunction(void* userPtr, enum RTCError error, const char* str)
{
    printf("error %d: %s\n", error, str);
}

RTCDevice initializeDevice()
{
    RTCDevice device = rtcNewDevice(NULL);

    if (!device)
        printf("error %d: cannot create device\n", rtcGetDeviceError(NULL));

    rtcSetDeviceErrorFunction(device, errorFunction, NULL);
    return device;
}

extern "C" void abort();
extern "C" void exit(int);
extern "C" int puts(const char* str);
extern "C" int putchar(int character);


float clamp(float n, float lower, float upper) {
    return std::max(lower, std::min(n, upper));
}


float generate_random_float()
{
    unsigned int tofloat = 0x2f800004u;
    static unsigned long x = 123456789, y = 362436069, z = 521288629;

    unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    float a = static_cast<float>(z)* reinterpret_cast<const float&>(tofloat);

    return a;
}


vec3 generate_random_vector()
{
    float t = generate_random_float();
    vec3 random(t);
    return random;
}


template <typename T>
void
substract_vector(std::vector<T>& a, const std::vector<T>& b)
{
    typename std::vector<T>::iterator       it = a.begin();
    typename std::vector<T>::const_iterator it2 = b.begin();

    while (it != a.end())
    {
        while (it2 != b.end() && it != a.end())
        {
            if (*it == *it2)
            {
                it = a.erase(it);
                it2 = b.begin();
            }

            else
                ++it2;
        }
        if (it != a.end())
            ++it;

        it2 = b.begin();
    }
}


inline float deg2rad(const float& deg)
{
    return deg * M_PI / 180;
}


inline float rad2deg(const float& rad)
{
    return rad * 180 / M_PI;
}

/*
template <typename T>
T
lerp(T& a, T& b, float& t)
{
    return (1 - t) * a + t * b;
}
*/

template <typename T>
T
lerp(T a, T b, float t)
{
    return (1 - t) * a + t * b;
}


vec3 random_ray_in_hemisphere(vec3& hit_normal)
{
    double r1 = generate_random_float();
    double r2 = generate_random_float();

    vec3 rand_dir_local(cos(2 * M_PI * r1) * sqrt(1 - r2), sin(2 * M_PI * r1) * sqrt(1 - r2), sqrt(1 - r1));
    vec3 rand(generate_random_float() - 0.5, generate_random_float() - 0.5, generate_random_float() - 0.5);

    vec3 tan1 = cross(hit_normal, rand);
    vec3 tan2 = cross(tan1.normalize(), hit_normal);

    vec3 rand_ray_dir = rand_dir_local.z * hit_normal + rand_dir_local.x * tan1 + rand_dir_local.y * tan2;

    return rand_ray_dir;
}


vec3 random_in_unit_disk()
{
    vec3 p;
    do {
        p = 2.0 * vec3(generate_random_float(), generate_random_float(), 0) - vec3(1.0f, 1.0f, 0.0f);
    } while (dot(p, p) >= 1);
    return p;
}



vec3 reflect(vec3& i, vec3& n, float R)
{
    vec3 random = random_ray_in_hemisphere(n);
    vec3 r = i - 2 * dot(i, n) * n + lerp(vec3(1.0f), random, R);
    return r.normalize();
}


vec3 refract(vec3& i, vec3& n, float ior)
{
    vec3 T(0.f);
    vec3 n_t = n.normalize();
    i = i.normalize();
    float n1 = 1.0;
    float n2 = ior;
    bool in = true;

    if (dot(i, n) > 0.0)
    {
        n1 = ior;
        n2 = 1.0;
        n_t = -n;
        in = false;
    }

    float dt = dot(i, n_t);
    float n_ior = n1 / n2;

    float radical = 1.0 - (n_ior * n_ior) * (1.0 - dt * dt);

    if (radical > 0.0)
    {
        //T = (n1 / n2) * (i.normalize() - dt * n_t) - n_t * sqrt(radical);
        T = n_ior * i + (n_ior * -dt - sqrt(radical)) * n_t;

        float r0 = ((n1 - n2) / (n1 + n2)) * ((n1 - n2) / (n1 + n2));
        float r;

        
        if (in) r = r0 + (1 - r0) * std::pow(1 + dot(i, n), 5);
        else r = r0 + (1 - r0) * std::pow(1 - dot(T, n), 5);

        if (generate_random_float() < r) T = reflect(i, n, 0.0f);
        else T = T; 
    }
    else T = reflect(i, n, 0.0f);
    
    return T;
}


float modulo(float x)
{
    return x - std::floor(x);
}


float estimate_sample_variance(vec3 samples[], int n)
{
    vec3 sum = 0;
    vec3 sum_sq = 0;

    for (int i = 0; i < n; i++)
    {
        sum += samples[i];
        sum_sq += samples[i] * samples[i];
    }

    float var_sum = sum_sq.x / (n * (n - 1)) - sum.x * sum.x / ((n - 1) * n * n) +
        sum_sq.y / (n * (n - 1)) - sum.y * sum.y / ((n - 1) * n * n) +
        sum_sq.z / (n * (n - 1)) - sum.z * sum.z / ((n - 1) * n * n);

    return var_sum / 3;
}


inline vec3 face_forward(const vec3& dir, const vec3& _Ng) {
    const vec3 Ng = _Ng;
    return dot(dir, Ng) < 0.0f ? Ng : Ng * -1;
}


float invsqrt(float number)
{
    union {
        float f;
        uint32_t i;
    } conv;

    float x2;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    conv.f = number;
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f = conv.f * (threehalfs - (x2 * conv.f * conv.f));
    return conv.f;
}


float IntegrateEdge(vec3 v1, vec3 v2)
{
    float cosTheta = dot(v1, v2);
    if (cosTheta > 0.99999)
        return 0.0;

    float theta = acos(cosTheta);
    float res = cross(v1, v2).z * theta * invsqrt(1.0 - cosTheta * cosTheta);

    return res;
}


float normalize2d(float x, float y)
{
    float length = sqrt((x * x) + (y * y));
    return (x / length + y / length) / 2;
}


typedef struct
{
    GLfloat R, G, B;
} color_t;

/*
void drawImage(GLuint file,
    float x,
    float y,
    float w,
    float h,
    float angle)
{
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);

    glBindTexture(GL_TEXTURE_2D, file);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(x, y, 0.0f);
    glTexCoord2f(0.0, 2.0); glVertex3f(x, y + h, 0.0f);
    glTexCoord2f(2.0, 2.0); glVertex3f(x + w, y + h, 0.0f);
    glTexCoord2f(2.0, 0.0); glVertex3f(x + w, y, 0.0f);
    glEnd();

    glPopMatrix();
}
*/

vec3 max(float a, vec3 b)
{
    return vec3(std::max(a, b.x), std::max(a, b.y), std::max(a, b.z));
}


vec3 HableToneMap(vec3 color)
{
    float A = 0.22; // Shoulder Strength
    float B = 0.30; // Linear Strength
    float C = 0.10; // Linear Angle
    float D = 0.20; // Toe Strength
    float E = 0.01; // Toe Numerator
    float F = 0.30; // Toe Denominator

    color = max(0, color - 0.004f);
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - (E / F);
    return color;
}


void reset_render(color_t* pixels, color_t* new_pixels, int xres, int yres, int& s)
{
    for (int y = 0; y < yres; y++)
    {
        for (int x = 0; x < xres; x++)
        {
            new_pixels[x + y * xres].R = 0.0f;
            new_pixels[x + y * xres].G = 0.0f;
            new_pixels[x + y * xres].B = 0.0f;
        }
    }

    for (int y = 0; y < yres; y++)
    {
        for (int x = 0; x < xres; x++)
        {
            pixels[x + y * xres].R = 0.0f;
            pixels[x + y * xres].G = 0.0f;
            pixels[x + y * xres].B = 0.0f;
        }
    }

    s = 1;
}


#endif