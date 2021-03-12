#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>

#include "vec2.h"
#include "vec3.h"
#include "sampling_utils.h"

#ifndef MATHS_UTILS
#define MATHS_UTILS

#define PLUS_INF std::numeric_limits<float>::infinity();
#define MIN_INF -std::numeric_limits<float>::infinity();
#define INV_LOG2 1.442695040888963387004650940071


// helper functions
template <typename T>
inline T fit(T& s, T a1, T a2, T b1, T b2) { return b1 + ((s - a1) * (b2 - b1)) / (a2 - a1); }

template <typename T>
inline T fit01(T& x, T a, T b) { return x * (b - a) + a; }

template <typename T>
inline T lerp(T a, T b, float t) { return (1 - t) * a + t * b; }

template <typename T>
inline T clamp(T n, float lower, float upper) { return std::max(lower, std::min(n, upper));  }

inline float deg2rad(const float deg) { return deg * M_PI / 180; }
inline float rad2deg(const float rad) { return rad * 180 / M_PI; }

inline float log2(float x) { return std::log(x) * INV_LOG2; }
inline float square(float x) { return x * x; }
inline float modulo(float x) { return x - std::floor(x); }

vec3 to_polar(const vec2& uv) {}


// trigonometry functions
inline float CosTheta(const vec3& w) { return w.z; }
inline float Cos2Theta(const vec3& w) { return w.z * w.z; }
inline float AbsCosTheta(const vec3& w) { return std::abs(w.z); }
inline float Sin2Theta(const vec3& w) { return std::max(0.0f, 1.0f - Cos2Theta(w)); }
inline float SinTheta(const vec3& w) { return sqrt(Sin2Theta(w)); }
inline float TanTheta(const vec3& w) { return SinTheta(w) / CosTheta(w); }
inline float Tan2Theta(const vec3& w) { return Sin2Theta(w) / Cos2Theta(w); }
inline float CosPhi(const vec3& w) { float sinTheta = SinTheta(w); return (sinTheta == 0) ? 1 : clamp(w.x / sinTheta, -1.0f, 1.0f); }
inline float SinPhi(const vec3& w) { float sinTheta = SinTheta(w); return (sinTheta == 0) ? 0 : clamp(w.y / sinTheta, -1.0f, 1.0f); }
inline float Cos2Phi(const vec3& w) { return CosPhi(w) * CosPhi(w); }
inline float Sin2Phi(const vec3& w) { return SinPhi(w) * SinPhi(w); }
inline float CosDPhi(const vec3& wa, const vec3& wb) { return clamp((wa.x * wb.x + wa.y * wb.y) / sqrt((wa.x * wa.x + wa.y * wa.y) * (wb.x * wb.x + wb.y * wb.y)), -1.0f, 1.0f); }

void createBasis(const vec3& hit_normal, vec3& tangent, vec3& bitangent);
void worldToTangent(const vec3& hit_normal, const vec3& tangent, const vec3& bitangent, const vec3& l, const vec3& v, vec3& wo, vec3& wi, vec3& wm);
vec3 tangentToWorld(const vec3& hit_normal, const vec3& tangent, const vec3& bitangent, const vec3& t);


// schlick/fresnels functions
vec3 schlick_weight(const vec3& f0, const float& h);
float schlick_r0_from_relative_ior(const float eta);
float fresnel_reflection_coef(float& n2, vec3& normal, vec3& incident);


// approx functions
float approx_acos(float x);
float approx_atan(float z);
float approx_atan2(float y, float x);
float inv_sqrt(float number);


// pathtracing utils functions
vec3 reflect(const vec3& i, const vec3& n);
vec3 refract(vec3& i, vec3& n, float ior);
vec3 face_forward(const vec3& dir, const vec3& _Ng);

#endif