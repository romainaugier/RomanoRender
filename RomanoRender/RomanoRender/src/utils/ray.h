#pragma once
#include "utils/vec3.h"

#ifndef RAY
#define RAY

class ray
{
public:
	ray() {}
	ray(const Vec3& a, const Vec3& b) : A(a), B(b)
	{
		invdir = 1 / B;
		sign[0] = (invdir.x < 0);
		sign[1] = (invdir.y < 0);
		sign[2] = (invdir.z < 0);
	}

	Vec3 origin() const { return A; }
	Vec3 direction() const { return B.normalize(); }
	Vec3 pt(float t) const { return A + B * t; }

public:
	Vec3 A, B;
	Vec3 invdir;
	int sign[3];
};

#endif