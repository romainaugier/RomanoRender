#pragma once
#include <iostream>

struct vec3
{
	//3 dimensions
	float x, y, z;

	//constructors
	vec3() {}
	vec3(float X, float Y, float Z) :x(X), y(Y), z(Z) {}


	//operators
	const vec3 operator+(const vec3& other) const
	{
		return vec3(x + other.x, y + other.y, z + other.z);
	}

	const vec3 operator+(const float& t) const
	{
		return vec3(x + t, y + t, z + t);
	}

	const vec3 operator-(const vec3& other) const
	{
		return vec3(x - other.x, y - other.y, z - other.z);
	}

	const vec3 operator-(const float& t) const
	{
		return vec3(x - t, y - t, z - t);
	}

	const vec3 operator*(const vec3& other) const
	{
		return vec3(x * other.x, y * other.y, z * other.z);
	}

	const vec3 operator*(const float& t) const
	{
		return vec3(x * t, y * t, z * t);
	}

	const vec3 operator/(const vec3& other) const
	{
		return vec3(x / other.x, y / other.y, z / other.z);
	}

	const vec3 operator/(const float& t) const
	{
		return vec3(x / t, y / t, z / t);
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
