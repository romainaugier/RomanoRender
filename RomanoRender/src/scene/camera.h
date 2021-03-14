#pragma once

#include <string>

#include "utils/ray.h"
#include "utils/vec3.h"
#include "utils/maths_utils.h"


#ifndef CAMERA
#define CAMERA

class Camera
{
public:
	Camera() {}

	Camera(vec3 _pos, vec3 _lookat, float focal, int xres, int yres, float aper, float focusdist, float anx, float any) :
		pos(_pos),
		lookat(_lookat),
		focal_length(focal),
		aspect((float)xres / (float)yres),
		aperture(aper),
		focus_dist(focusdist),
		anamorphic_x(anx),
		anamorphic_y(any)
	{
		vec3 u, v, w;
		vec3 up(0.0f, 1.0f, 0.0f);

		fov = 2 * rad2deg(std::atan(36.0f / (2 * focal_length)));

		float theta = fov * M_PI / 180.0f;
		float half_height = tan(theta / 2.0f);
		float half_width = aspect * half_height;

		origin = pos;
		
		vec3 w_(pos - lookat);
		w = w_.normalize();
		u = cross(w, up).normalize();
		v = cross(w, u);
		
		lower_left_corner = origin - u * half_width - v * half_height - w;
		h = u * 2.0f * half_width;
		v = v * 2.0F * half_height;

	}

	Ray get_ray(float s, float t);

	void update(int& xres, int& yres);

public:
	vec3 origin;
	vec3 lower_left_corner;
	vec3 h, v;
	vec3 pos;
	vec3 lookat;

	float focal_length;
	float fov;
	float aspect;
	float aperture;
	float focus_dist;
	float anamorphic_x;
	float anamorphic_y;

	std::string name;
};

#endif