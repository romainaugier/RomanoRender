#pragma once
#include "utils/vec3.h"
#include "embree_utils.h"

#ifndef RAY
#define RAY


class Ray
{
public:
	Ray() {}

	Ray(const vec3& a, const vec3& b) : origin(a), direction(b)
	{
        vec3 dir = direction.normalize();

        //rayhit struct
        rayhit.ray.org_x = origin.x;
        rayhit.ray.org_y = origin.y;
        rayhit.ray.org_z = origin.z;
        rayhit.ray.dir_x = dir.x;
        rayhit.ray.dir_y = dir.y;
        rayhit.ray.dir_z = dir.z;
        rayhit.ray.tnear = 0.01f;
        rayhit.ray.tfar = 10000.0f;
        rayhit.ray.mask = -1;
        rayhit.ray.flags = 0;
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

        //simple ray struct
        ray.org_x = origin.x;
        ray.org_y = origin.y;
        ray.org_z = origin.z;
        ray.dir_x = dir.x;
        ray.dir_y = dir.y;
        ray.dir_z = dir.z;
        ray.tnear = 0.001f;
        ray.tfar = 10000.0f;
        ray.mask = -1;
        ray.flags = 0;
	}

    Ray(const vec3& a, const vec3& b, const float n_clip, const float f_clip) : origin(a), direction(b)
    {
        vec3 dir = direction.normalize();

        //rayhit struct
        rayhit.ray.org_x = origin.x;
        rayhit.ray.org_y = origin.y;
        rayhit.ray.org_z = origin.z;
        rayhit.ray.dir_x = dir.x;
        rayhit.ray.dir_y = dir.y;
        rayhit.ray.dir_z = dir.z;
        rayhit.ray.tnear = n_clip;
        rayhit.ray.tfar = f_clip;
        rayhit.ray.mask = -1;
        rayhit.ray.flags = 0;
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

        //simple ray struct
        ray.org_x = origin.x;
        ray.org_y = origin.y;
        ray.org_z = origin.z;
        ray.dir_x = dir.x;
        ray.dir_y = dir.y;
        ray.dir_z = dir.z;
        ray.tnear = n_clip;
        ray.tfar = f_clip;
        ray.mask = -1;
        ray.flags = 0;
    }

public:
    vec3 origin;
    vec3 direction;

	RTCRayHit rayhit;
    RTCRay ray;
};


#endif