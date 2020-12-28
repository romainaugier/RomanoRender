#pragma once
#include "utils/vec3.h"

#ifndef RAY
#define RAY

class ray
{
public:
	ray() {}
	ray(const vec3& a, const vec3& b) : A(a), B(b)
	{
		invdir = 1 / B;
		sign[0] = (invdir.x < 0);
		sign[1] = (invdir.y < 0);
		sign[2] = (invdir.z < 0);
	}

	vec3 origin() const { return A; }
	vec3 direction() const { return B.normalize(); }
	vec3 pt(float t) const { return A + B * t; }

public:
	vec3 A, B;
	vec3 invdir;
	int sign[3];
};

#endif