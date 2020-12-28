#pragma once
#include <iostream>

#ifndef VEC3
#define VEC3

struct alignas(16) vec3
{
	//3 dimensions
	float x, y, z, a;

	//constructors
	vec3() {}
	vec3(float X, float Y, float Z) :x(X), y(Y), z(Z) {}
	vec3(float t) : x(t), y(t), z(t) {}

	vec3 operator-()
	{
		return vec3(-x, -y, -z);
	}


	//functions
	float length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	float length2() const
	{
		return x * x + y * y + z * z;
	}

	vec3 normalize() const
	{
		float t = 1 / length();
		return vec3(x * t, y * t, z * t);

	}
};

std::ostream& operator<<(std::ostream& out, const vec3& v)
{
	out << v.x << ", " << v.y << ", " << v.z << std::endl;
	return out;
}


//operators
const vec3 operator+(const vec3& vec, const vec3& other)
{
	return vec3(vec.x + other.x, vec.y + other.y, vec.z + other.z);
}

const vec3 operator+(const vec3& vec, const float& t)
{
	return vec3(vec.x + t, vec.y + t, vec.z + t);
}

const vec3 operator-(const vec3& vec, const vec3& other)
{
	return vec3(vec.x - other.x, vec.y - other.y, vec.z - other.z);
}

const vec3 operator-(const vec3& vec, const float& t)
{
	return vec3(vec.x - t, vec.y - t, vec.z - t);
}

const vec3 operator*(const vec3& vec, const vec3& other)
{
	return vec3(vec.x * other.x, vec.y * other.y, vec.z * other.z);
}

const vec3 operator*(const vec3& vec, const float& t)
{
	return vec3(vec.x * t, vec.y * t, vec.z * t);
}

const vec3 operator/(const vec3& vec, const vec3& other)
{
	return vec3(vec.x / other.x, vec.y / other.y, vec.z / other.z);
}

const vec3 operator/(const vec3& vec, const float& t) 
{
	return vec3(vec.x / t, vec.y / t, vec.z / t);
}

vec3 operator/(int& t, vec3& n)
{
	return vec3(n.x / t, n.y / t, n.z / t);
}

const bool operator==(const vec3& v0, const vec3& v1)
{
	if (v0.x == v1.x && v0.y == v1.y && v0.z == v1.z) return true;
	else return false;
}

bool operator==(vec3& v0, vec3& v1)
{
	if (v0.x == v1.x && v0.y == v1.y && v0.z == v1.z) return true;
	else return false;
}

vec3 operator+=(vec3& v0, vec3 v1)
{
	v0 = v0 + v1;
	return v0;
}

bool operator>(vec3& v0, vec3& v1)
{
	if (v0.x > v1.x && v0.y > v1.y && v0.z > v1.z) return true;
	else return false;
}

const float dot(const vec3& v1, const vec3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

const vec3 cross(const vec3& v1, const vec3& v2)
{
	return vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

float length(vec3 v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3 sum(const vec3& v1, const vec3& v2, const vec3& v3)
{
	return vec3((v1.x + v2.x + v3.x) / 3, (v1.y + v2.y + v3.y) / 3, (v1.z + v2.z + v3.z) / 3);
}

float dist(vec3& a, vec3& b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

vec3 powvec3(vec3 v, float p)
{
	return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}

vec3 rotation_y(vec3 v, float a)
{
	vec3 r(0.0f);
	r.x = v.x * cos(a) + v.z * sin(a);
	r.y = v.y;
	r.z = -v.x * sin(a) + v.z * cos(a);
	return r;
}

vec3 rotation_z(vec3 v, float a)
{
	vec3 r(0.0f);
	r.x = v.x * cos(a) - v.y * sin(a);
	r.y = v.x * sin(a) + v.y * cos(a);
	r.z = v.z;
	return r;
}

vec3 rotation_yz(vec3 v, float ay, float az)
{
	return rotation_z(rotation_y(v, ay), az);
}


#endif