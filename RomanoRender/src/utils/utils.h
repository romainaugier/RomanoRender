#pragma once
#include <vector>
#include <random>
#include "GLFW/glfw3.h"
#include "matrix.h"
#include "scene/settings.h"
#include "vec2.h"
#include "Tracy.hpp"

#ifndef UTILS
#define UTILS


inline float estimate_sample_variance(vec3 samples[], int n)
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



typedef struct { GLfloat R, G, B; } color_t;

/*
vec3 max(float a, vec3 b)
{
    return vec3(std::max(a, b.x), std::max(a, b.y), std::max(a, b.z));
}
*/

/*
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
*/


inline void reset_render(color_t* pixels, color_t* new_pixels, int xres, int yres, int& s, int& y)
{
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
}


inline auto get_time()
{
    auto start = std::chrono::system_clock::now();
    return start;
}




#define  Pr  .299
#define  Pg  .587
#define  Pb  .114

inline vec3 changeSaturation(vec3& color, float change) {

    double  P = sqrt(
        (color.x) * (color.x) * Pr +
        (color.y) * (color.y) * Pg +
        (color.z) * (color.z) * Pb);

    color.x = P + ((color.x) - P) * change;
    color.y = P + ((color.y) - P) * change;
    color.z = P + ((color.z) - P) * change;

    return color;
}




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

/*
float roughness_to_alpha(float roughness) {
    roughness = std::max(roughness, (float)1e-3);
    float x = std::log(roughness);
    return 1.62142f + 0.819955f * x + 0.1734f * x * x +
        0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
}
*/


inline float power_heuristic(int nf, float fPdf, int ng, float gPdf) {
    float f = nf * fPdf, g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}








#endif

