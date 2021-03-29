#pragma once
#include <iostream>

#ifndef VEC3
#define VEC3

inline float rsqrt(float num);

struct alignas(16) vec3
{
	float x, y, z, a;

	vec3() {}
	vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
	vec3(float t) : x(t), y(t), z(t) {}

	const vec3 operator-() const { return vec3(-x, -y, -z); }

	const float& operator [] (int i) const { return (&x)[i]; }
	float& operator [] (int i) { return (&x)[i]; }

	float length() const { return sqrtf(x * x + y * y + z * z); }
	float length2() const { return x * x + y * y + z * z; }
	vec3 q_normalize() const { float t = rsqrt(x * x + y * y + z * z); return vec3(x * t, y * t, z * t); }
	vec3 normalize() const { float t = 1.0f / length(); return vec3(x * t, y * t, z * t); }
};


inline float rsqrt(float num)
{
	int i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = num * 0.5f;
	y = num;

	memcpy(&i, &y, sizeof(float));
	i = 0x5f3759df - (i >> 1);

	memcpy(&y, &i, sizeof(float));
	y = y * (threehalfs - (x2 * y * y));

	return y;
}


// ostream operator for easier cout
inline std::ostream& operator<<(std::ostream& out, const vec3& v) { out << v.x << ", " << v.y << ", " << v.z << std::endl; return out; }


// maths operators
inline vec3 operator+(const vec3& vec, const vec3& other) { return vec3(vec.x + other.x, vec.y + other.y, vec.z + other.z); }
inline vec3 operator+(const vec3& vec, const float t) { return vec3(vec.x + t, vec.y + t, vec.z + t); }
inline vec3 operator-(const vec3& vec, const vec3& other) { return vec3(vec.x - other.x, vec.y - other.y, vec.z - other.z); }
inline vec3 operator-(const vec3& vec, const float t) { return vec3(vec.x - t, vec.y - t, vec.z - t); }
inline vec3 operator*(const vec3& vec, const vec3& other) { return vec3(vec.x * other.x, vec.y * other.y, vec.z * other.z); }
inline vec3 operator*(const vec3& vec, const float t) { return vec3(vec.x * t, vec.y * t, vec.z * t); }
inline vec3 operator*(const float t, const vec3& vec) { return vec3(vec.x * t, vec.y * t, vec.z * t); }
inline vec3 operator/(const vec3& vec, const vec3& other) { return vec3(vec.x / other.x, vec.y / other.y, vec.z / other.z); }
inline vec3 operator/(const vec3& vec, const float t) { return vec3(vec.x / t, vec.y / t, vec.z / t); }
inline vec3 operator/(const float t, const vec3& vec) { return vec3(vec.x / t, vec.y / t, vec.z / t); }

inline bool operator==(const vec3& v0, const vec3& v1) { if (v0.x == v1.x && v0.y == v1.y && v0.z == v1.z) return true; else return false; }
inline bool operator>(const vec3& v0, const vec3& v1) { if (v0.x > v1.x && v0.y > v1.y && v0.z > v1.z) return true; else return false; }
inline bool operator>=(const vec3& v0, const vec3& v1) { if (v0.x >= v1.x && v0.y >= v1.y && v0.z >= v1.z) return true; else return false; }
inline bool operator<(const vec3& v0, const vec3& v1) { if (v0.x < v1.x && v0.y < v1.y && v0.z < v1.z) return true; else return false; }
inline bool operator<=(const vec3& v0, const vec3& v1) { if (v0.x <= v1.x && v0.y <= v1.y && v0.z <= v1.z) return true; else return false; }

inline vec3 operator+=(vec3& v0, vec3 v1) { v0 = v0 + v1; return v0; }
inline vec3 operator-=(vec3& v0, vec3 v1) { v0 = v0 - v1; return v0; }
inline vec3 operator*=(vec3& v0, vec3 v1) { v0 = v0 * v1; return v0; }
inline vec3 operator*=(vec3& v0, float t) { v0 = v0 * t; return v0; }
inline vec3 operator/=(vec3& v0, vec3 v1) { v0 = v0 / v1; return v0; }
inline vec3 operator/=(vec3& v0, float t) { v0 = v0 / t; return v0; }


// utility functions
inline float dot(const vec3& v1, const vec3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
inline vec3 cross(const vec3& v1, const vec3& v2) { return vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }
inline float dist(const vec3& a, const vec3& b) { return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z)); }
inline vec3 abs(const vec3& a) { return vec3(abs(a.x), abs(a.y), abs(a.z)); }

inline vec3 sum(const vec3& v1, const vec3& v2, const vec3& v3) { return vec3((v1.x + v2.x + v3.x) / 3, (v1.y + v2.y + v3.y) / 3, (v1.z + v2.z + v3.z) / 3); }
inline vec3 powvec3(vec3 v, float p) { return vec3(powf(v.x, p), powf(v.y, p), powf(v.z, p)); }


#endif