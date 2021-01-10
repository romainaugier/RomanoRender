#pragma once
#include "utils/ray.h"
#include "utils/vec3.h"
#include "utils/utils.h"


#ifndef CAMERA
#define CAMERA

class camera
{
public:
	camera() {}

	camera(Vec3 _pos, Vec3 _lookat, float focal, int xres, int yres, float aper, float focusdist, float anx, float any) :
		pos(_pos),
		lookat(_lookat),
		focal_length(focal),
		aspect((float)xres / (float)yres),
		aperture(aper),
		focus_dist(focusdist),
		anamorphic_x(anx),
		anamorphic_y(any)
	{
		Vec3 u, v, w;
		Vec3 up(0.0f, 1.0f, 0.0f);

		fov = 2 * rad2deg(std::atan(36.0f / (2 * focal_length)));

		float theta = fov * M_PI / 180.0f;
		float half_height = tan(theta / 2.0f);
		float half_width = aspect * half_height;

		origin = pos;
		
		Vec3 w_(pos - lookat);
		w = w_.normalize();
		u = cross(w, up).normalize();
		v = cross(w, u);
		
		lower_left_corner = origin - u * half_width - v * half_height - w;
		h = u * 2.0f * half_width;
		v = v * 2.0F * half_height;

	}

	ray get_ray(float s, float t) 
	{
		return ray(origin, lower_left_corner + h * s + v * t - origin);
	}

	void update(int& xres, int& yres)
	{
		aspect = (float)xres / (float)yres;

		Vec3 u, w;
		Vec3 up(0.0f, 1.0f, 0.0f);

		fov = 2 * rad2deg(std::atan(36.0f / (2 * focal_length)));

		float theta = fov * M_PI / 180.0f;
		float half_height = tan(theta / 2.0f);
		float half_width = aspect * half_height;

		origin = pos;

		Vec3 w_(pos - lookat);
		w = w_.normalize();
		u = cross(w, up).normalize();
		v = cross(w, u);

		lower_left_corner = origin - u * half_width - v * half_height - w;
		h = u * 2.0f * half_width;
		v = v * 2.0F * half_height;
	}

public:
	Vec3 origin;
	Vec3 lower_left_corner;
	Vec3 h, v;
	Vec3 pos;
	Vec3 lookat;

	float focal_length;
	float fov;
	float aspect;
	float aperture;
	float focus_dist;
	float anamorphic_x;
	float anamorphic_y;
};

#endif