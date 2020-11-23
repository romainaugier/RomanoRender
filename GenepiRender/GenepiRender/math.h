#pragma once
#include "vec3.h"

void createBasis(vec3& hit_normal, vec3& tangent, vec3& bitangent)
{
	vec3 up(0.0f, 1.0f, 0.0f);
	if (dot(up, hit_normal) > 0.9f) up = vec3(0.0f, 0.0f, 1.0f);

	tangent = cross(hit_normal, up).normalize();
	bitangent = cross(hit_normal, tangent);
}


void worldToTangent(vec3& hit_normal, vec3& tangent, vec3& bitangent, vec3& l, vec3& v, vec3& wo, vec3& wi, vec3& wm)
{
	wo = (v.z * hit_normal + v.x * tangent + v.y * bitangent);
	wi = (l.z * hit_normal + l.x * tangent + v.y * bitangent);
	wm = (wo + wi);
}


vec3 tangentToWorld(vec3& hit_normal, vec3& tangent, vec3& bitangent, vec3& t)
{
	vec3 wdir;
	wdir = tangent * t.x + hit_normal * t.y + bitangent * t.z;
	return wdir;
}