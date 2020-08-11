#pragma once
#include "vec3.h"

class ray
{
public:
	ray() {}
	ray(const vec3& a, const vec3& b) { A = a; B = b; }
	vec3 origin() const { return A; }
	vec3 direction() const { return B.normalize(); }
	vec3 pt(float t) const { return A + B * t; }

private:
	vec3 A, B;

};