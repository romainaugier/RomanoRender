#pragma once
#include <iostream>
#include <smmintrin.h>


#ifndef VEC3
#define VEC3

struct alignas(16) Vec3
{
	//3 dimensions
	float x, y, z, a;

	//constructors
	inline Vec3() {}
	inline Vec3(float X, float Y, float Z) :x(X), y(Y), z(Z) {}
	inline Vec3(float t) : x(t), y(t), z(t) {}


	Vec3 operator-()
	{
		return Vec3(-x, -y, -z);
	}


	//float& operator [] (uint8_t i) { return (i == 0) ? x : (i == 1) ? y : z; }

	const float& operator [] (uint8_t i) const { return (&x)[i]; }
	float& operator [] (uint8_t i) { return (&x)[i]; }

	//functions
	float length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	float length2() const
	{
		return x * x + y * y + z * z;
	}

	Vec3 normalize() const
	{
		float t = 1 / length();
		return Vec3(x * t, y * t, z * t);

	}
};


std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
	out << v.x << ", " << v.y << ", " << v.z << std::endl;
	return out;
}





//operators
const Vec3 operator+(const Vec3& vec, const Vec3& other)
{
	return Vec3(vec.x + other.x, vec.y + other.y, vec.z + other.z);
}

const Vec3 operator+(const Vec3& vec, const float& t)
{
	return Vec3(vec.x + t, vec.y + t, vec.z + t);
}

const Vec3 operator-(const Vec3& vec, const Vec3& other)
{
	return Vec3(vec.x - other.x, vec.y - other.y, vec.z - other.z);
}

const Vec3 operator-(const Vec3& vec, const float& t)
{
	return Vec3(vec.x - t, vec.y - t, vec.z - t);
}

const Vec3 operator*(const Vec3& vec, const Vec3& other)
{
	return Vec3(vec.x * other.x, vec.y * other.y, vec.z * other.z);
}

const Vec3 operator*(const Vec3& vec, const float& t)
{
	return Vec3(vec.x * t, vec.y * t, vec.z * t);
}

const Vec3 operator/(const Vec3& vec, const Vec3& other)
{
	return Vec3(vec.x / other.x, vec.y / other.y, vec.z / other.z);
}

const Vec3 operator/(const Vec3& vec, const float& t) 
{
	return Vec3(vec.x / t, vec.y / t, vec.z / t);
}

Vec3 operator/(int& t, Vec3& n)
{
	return Vec3(n.x / t, n.y / t, n.z / t);
}

const bool operator==(const Vec3& v0, const Vec3& v1)
{
	if (v0.x == v1.x && v0.y == v1.y && v0.z == v1.z) return true;
	else return false;
}

bool operator==(Vec3& v0, Vec3& v1)
{
	if (v0.x == v1.x && v0.y == v1.y && v0.z == v1.z) return true;
	else return false;
}

Vec3 operator+=(Vec3& v0, Vec3 v1)
{
	v0 = v0 + v1;
	return v0;
}

bool operator>(Vec3& v0, Vec3& v1)
{
	if (v0.x > v1.x && v0.y > v1.y && v0.z > v1.z) return true;
	else return false;
}

const float dot(const Vec3& v1, const Vec3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

const Vec3 cross(const Vec3& v1, const Vec3& v2)
{
	return Vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

float length(Vec3 v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vec3 sum(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
	return Vec3((v1.x + v2.x + v3.x) / 3, (v1.y + v2.y + v3.y) / 3, (v1.z + v2.z + v3.z) / 3);
}

float dist(Vec3& a, Vec3& b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

Vec3 powvec3(Vec3 v, float p)
{
	return Vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}

Vec3 rotation_y(Vec3 v, float a)
{
	Vec3 r(0.0f);
	r.x = v.x * cos(a) + v.z * sin(a);
	r.y = v.y;
	r.z = -v.x * sin(a) + v.z * cos(a);
	return r;
}

Vec3 rotation_z(Vec3 v, float a)
{
	Vec3 r(0.0f);
	r.x = v.x * cos(a) - v.y * sin(a);
	r.y = v.x * sin(a) + v.y * cos(a);
	r.z = v.z;
	return r;
}

Vec3 rotation_yz(Vec3 v, float ay, float az)
{
	return rotation_z(rotation_y(v, ay), az);
}


#endif