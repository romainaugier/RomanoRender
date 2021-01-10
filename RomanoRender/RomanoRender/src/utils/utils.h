#pragma once
#include <vector>
#include <random>
#include "GLFW/glfw3.h"
#include "matrix.h"
#include "vec2.h"
#include "Tracy.hpp"

#define _CRT_SECURE_NO_WARNINGS
#define INV_PI = 0.31830988618379067154

#ifndef UTILS
#define UTILS


struct Vertex { float x, y, z, r; };

struct Triangle { int v0, v1, v2; };

static float infinity = std::numeric_limits<float>::infinity();

#include <embree3/rtcore.h>

void errorFunction(void* userPtr, enum RTCError error, const char* str)
{
    printf("error %d: %s\n", error, str);
}


inline float fit01(float x, float a, float b) { return x * (b - a) + a; }


inline float fit(float s, float a1, float a2, float b1, float b2)
{
    return b1 + ((s - a1) * (b2 - b1)) / (a2 - a1);
}


RTCDevice initializeDevice()
{
    RTCDevice device = rtcNewDevice(NULL);

    if (!device)
        printf("error %d: cannot create device\n", rtcGetDeviceError(NULL));

    rtcSetDeviceErrorFunction(device, errorFunction, NULL);
    return device;
}


inline float clamp(float n, float lower, float upper) 
{
    return std::max(lower, std::min(n, upper));
}


Vec3 clamp(Vec3& n, float lower, float upper)
{
    return Vec3(std::max(lower, std::min(n.x, upper)), std::max(lower, std::min(n.y, upper)), std::max(lower, std::min(n.z, upper)));
}



float ApproxAcos(float x)
{
    float negate = float(x < 0);
    x = abs(x);
    float ret = -0.0187293;
    ret = ret * x;
    ret = ret + 0.0742610;
    ret = ret * x;
    ret = ret - 0.2121144;
    ret = ret * x;
    ret = ret + 1.5707288;
    ret = ret * sqrt(1.0 - x);
    ret = ret - 2 * negate * ret;
    return negate * 3.14159265358979 + ret;
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

//other constants to work with : 0xd2a98b26625eee7b 0xdf900294d8f554a5 0x170865df4b3201fc

int wang_hash(int seed)
{
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9u;
    seed = seed ^ (seed >> 4u);
    seed *= 0x27d4eb2du;
    seed = seed ^ (seed >> 15u);
    return 1u + seed;
}

int xorshift32(int state)
{
    int x = state;
    x ^= x << 13u;
    x ^= x >> 17u;
    x ^= x << 5u;
    return x;
}


__forceinline float generate_random_float_3(__int64 seed)
{
    unsigned int tofloat = 0x2f800004u;
    seed *= 0xdddf9b1090aa7ac1;
    seed += 0xd2a98b26625eee7b;
    return static_cast<float>(seed >> 32) * reinterpret_cast<const float&>(tofloat) + 0.5f;
}


float generate_random_float_2(int state)
{
    ZoneScoped;
    unsigned int tofloat = 0x2f800004u;
    state = wang_hash(state);
    int x = xorshift32(state);
    state = x;
    return static_cast<float>(x) * reinterpret_cast<const float&>(tofloat) + 0.5f;
}


Vec3 generate_random_vector()
{
    float t = generate_random_float();
    Vec3 random(t);
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


// snippet from criver#8473
//inline Vec3 sample_ray_in_hemisphere(Vec3& hit_normal, vec2& sample)
//{
//    float signZ = (hit_normal.z >= 0.0f) ? 1.0f : -1.0f;
//    float a = -1.0f / (signZ + hit_normal.z);
//    float b = hit_normal.x * hit_normal.y * a;
//    Vec3 b1 = Vec3(1.0f + signZ * hit_normal.x * hit_normal.x * a, signZ * b, -signZ * hit_normal.x);
//    Vec3 b2 = Vec3(b, signZ + hit_normal.y * hit_normal.y * a, -hit_normal.y);
//
//
//    float phi = 2.0f * M_PI * sample.x;
//    float cosTheta = sqrt(sample.y);
//    float sinTheta = sqrt(1.0f - sample.y);
//    return ((b1 * cosf(phi) + b2 * sinf(phi)) * cosTheta + hit_normal * sinTheta).normalize();
//}


Vec3 sample_ray_in_hemisphere(Vec3& hit_normal, const vec2& sample)
{
    float a = 1.0f - 2.0f * sample.x;
    float b = sqrtf(1.0f - a * a);
    float phi = 2.0f * M_PI * sample.y;

    return Vec3(hit_normal.x + b * cos(phi), hit_normal.y + b * sin(phi), hit_normal.z + a);
}


Vec3 sample_dome_light(Vec3& hit_normal, const vec2& sample)
{
    float a = 1.0f - 2.0f * sample.x;
    float b = sqrtf(1.0f - a * a);
    float phi = 2.0f * M_PI * sample.y;

    return Vec3(hit_normal.x + b * cos(phi), hit_normal.y + b * sin(phi), hit_normal.z + a);
}


Vec3 sample_ray_in_sphere()
{
    return Vec3(generate_random_float(), generate_random_float(), generate_random_float());
}


Vec3 random_in_unit_disk()
{
    Vec3 p;
    do {
        p = 2.0 * Vec3(generate_random_float(), generate_random_float(), 0) - Vec3(1.0f, 1.0f, 0.0f);
    } while (dot(p, p) >= 1);
    return p;
}



inline Vec3 reflect(Vec3& i, Vec3& n)
{
    return (i - 2 * dot(i, n) * n).normalize();
}


Vec3 refract(Vec3& i, Vec3& n, float ior)
{
    Vec3 T(0.f);
    Vec3 n_t = n.normalize();
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

        if (generate_random_float() < r) T = reflect(i, n);
        else T = T; 
    }
    else T = reflect(i, n);
    
    return T;
}


inline float modulo(float x)
{
    return x - std::floor(x);
}


float estimate_sample_variance(Vec3 samples[], int n)
{
    Vec3 sum = 0;
    Vec3 sum_sq = 0;

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


inline Vec3 face_forward(const Vec3& dir, const Vec3& _Ng) {
    const Vec3 Ng = _Ng;
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


float IntegrateEdge(Vec3 v1, Vec3 v2)
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


Vec3 max(float a, Vec3 b)
{
    return Vec3(std::max(a, b.x), std::max(a, b.y), std::max(a, b.z));
}


Vec3 HableToneMap(Vec3 color)
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


void reset_render(color_t* pixels, color_t* new_pixels, int xres, int yres, int& s, int& y, double time)
{
    /*
    for (int y = 0; y < yres; y++)
    {
        for (int x = 0; x < xres; x++)
        {
            new_pixels[x + y * xres].R = 0.0f;
            new_pixels[x + y * xres].G = 0.0f;
            new_pixels[x + y * xres].B = 0.0f;
        }
    }
    */
#pragma omp parallel for
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
    y = 0;
    time = 0;
}


auto get_time()
{
    auto start = std::chrono::system_clock::now();
    return start;
}


Vec3 vec_abs(Vec3& a)
{
    return Vec3(abs(a.x), abs(a.y), abs(a.z));
}


Vec3 toPolar(vec2& uv)
{
    float theta = 2.0 * M_PI * uv.x + -M_PI / 2.0;
    float phi = M_PI * uv.y;

    Vec3 n;

    n.x = cos(theta) * sin(phi);
    n.y = sin(theta) * sin(phi);
    n.z = cos(phi);

    //n = normalize(n);
    return n;
}


float exponential_distribution(float& sigma)
{
    float xi = generate_random_float() - 0.001f;
    return -logf(1.0f - xi) / sigma;
}



#define  Pr  .299
#define  Pg  .587
#define  Pb  .114

Vec3 changeSaturation(Vec3& color, float change) {

    double  P = sqrt(
        (color.x) * (color.x) * Pr +
        (color.y) * (color.y) * Pg +
        (color.z) * (color.z) * Pb);

    color.x = P + ((color.x) - P) * change;
    color.y = P + ((color.y) - P) * change;
    color.z = P + ((color.z) - P) * change;

    return color;
}


bool FileExists(const std::string& abs_filename) {
    bool ret;
    FILE* fp = fopen(abs_filename.c_str(), "rb");
    if (fp) {
        ret = true;
        fclose(fp);
    }
    else {
        ret = false;
    }

    return ret;
}


std::string GetBaseDir(const std::string& filepath) {
    if (filepath.find_last_of("/\\") != std::string::npos)
        return filepath.substr(0, filepath.find_last_of("/\\"));
    return "";
}



void createBasis(Vec3& hit_normal, Vec3& tangent, Vec3& bitangent)
{
    Vec3 up(0.0f, 1.0f, 0.0f);
    if (dot(up, hit_normal) > 0.9f) up = Vec3(0.0f, 0.0f, 1.0f);

    tangent = cross(hit_normal, up).normalize();
    bitangent = cross(hit_normal, tangent);
}


void worldToTangent(Vec3& hit_normal, Vec3& tangent, Vec3& bitangent, Vec3& l, Vec3& v, Vec3& wo, Vec3& wi, Vec3& wm)
{
    wo = (v.z * hit_normal + v.x * tangent + v.y * bitangent).normalize();
    wi = (l.z * hit_normal + l.x * tangent + v.y * bitangent).normalize();
    wm = (wo + wi).normalize();
}


Vec3 tangentToWorld(Vec3& hit_normal, Vec3& tangent, Vec3& bitangent, Vec3& t)
{
    Vec3 wdir;
    wdir = tangent * t.x + hit_normal * t.y + bitangent * t.z;
    return wdir;
}


inline float Log2(float x)
{
    const float invLog2 = 1.442695040888963387004650940071;
    return std::log(x) * invLog2;
}


inline float square(float x) { return x * x; }


inline float Saturate(float x)
{
    if (x < 0.0f) {
        return 0.0f;
    }
    else if (x > 1.0f) {
        return 1.0f;
    }

    return x;
}


static float inv_pi = 0.31830988618379067154;


// trig identities
inline float CosTheta(Vec3& w) { return w.z; }
inline float Cos2Theta(Vec3& w) { return w.z * w.z; }
inline float AbsCosTheta(Vec3& w) { return std::abs(w.z); }
inline float Sin2Theta(Vec3& w) { return std::max(0.0f, 1.0f - Cos2Theta(w)); }
inline float SinTheta(Vec3& w) { return sqrt(Sin2Theta(w)); }
inline float TanTheta(Vec3& w) { return SinTheta(w) / CosTheta(w); }
inline float Tan2Theta(Vec3& w) { return Sin2Theta(w) / Cos2Theta(w); }
inline float CosPhi(Vec3& w)
{
    float sinTheta = SinTheta(w);
    return (sinTheta == 0) ? 1 : clamp(w.x / sinTheta, -1.0f, 1.0f);
}
inline float SinPhi(Vec3& w)
{
    float sinTheta = SinTheta(w);
    return (sinTheta == 0) ? 0 : clamp(w.y / sinTheta, -1.0f, 1.0f);
}
inline float Cos2Phi(Vec3& w) { return CosPhi(w) * CosPhi(w); }
inline float Sin2Phi(Vec3& w) { return SinPhi(w) * SinPhi(w); }
inline float CosDPhi(Vec3& wa, Vec3& wb) { return clamp((wa.x * wb.x + wa.y * wb.y) / sqrt((wa.x * wa.x + wa.y * wa.y) * (wb.x * wb.x + wb.y * wb.y)), -1.0f, 1.0f); }


// Schlicks/Fresnels
inline Vec3 SchlickWeight(Vec3& f0, float& h) { return f0 + (1.0f - f0) * std::pow(1 - h, 5); }
inline float SchlickR0FromRelativeIOR(float eta) { return square(eta - 1.0f) / square(eta + 1.0f); }


float FresnelReflectionCoef(float& n2, Vec3& normal, Vec3& incident)
{
    float n1 = 1.0f;
    float r0 = (n1 - n2) / (n1 + n2);
    float cosX = -dot(normal, incident);

    if (n1 > n2)
    {
        float n = n1 / n2;
        float sint2 = n * n * (1.0f - cosX * cosX);

        if (sint2 > 0.9999f) return 1.0f;
        cosX = sqrt(1.0f - sint2);
    }
    float x = 1.0f - cosX;
    return r0 + (1.0f - r0) * x * x * x * x * x;
}


float RoughnessToAlpha(float roughness) {
    roughness = std::max(roughness, (float)1e-3);
    float x = std::log(roughness);
    return 1.62142f + 0.819955f * x + 0.1734f * x * x +
        0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
}


inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf) {
    float f = nf * fPdf, g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}


inline float ApproxAtan(float z)
{
    const float n1 = 0.97239411f;
    const float n2 = -0.19194795f;
    return (n1 + n2 * z * z) * z;
}


float ApproxAtan2(float y, float x)
{
    const float n1 = 0.97239411f;
    const float n2 = -0.19194795f;
    float result = 0.0f;
    if (x != 0.0f)
    {
        const union { float flVal; int nVal; } tYSign = { y };
        const union { float flVal; int nVal; } tXSign = { x };
        if (fabsf(x) >= fabsf(y))
        {
            union { float flVal; int nVal; } tOffset = { M_PI };
            tOffset.nVal *= tYSign.nVal & 0x80000000u;
            tOffset.nVal *= tXSign.nVal >> 31;
            result = tOffset.flVal;
            const float z = y / x;
            result += (n1 + n2 * z * z) * z;
        }
        else
        {
            union { float flVal; int nVal; } tOffset = { M_PI_2 };
            tOffset.nVal |= tYSign.nVal & 0x80000000u;
            result = tOffset.flVal;
            const float z = x / y;
            result -= (n1 + n2 * z * z) * z;
        }
    }
    else if (y > 0.0f)
    {
        result = M_PI_2;
    }
    else if (y < 0.0f)
    {
        result = -M_PI_2;
    }
    return result;
}


#endif

