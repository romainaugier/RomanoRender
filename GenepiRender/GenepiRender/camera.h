#pragma once
#include "ray.h"
#include "vec3.h"
//#define M_PI = 3.14159265359

class camera
{
public:
	camera(vec3 &pos, vec3 &lookat, float &fov, float &aspect) {
		vec3 u, v, w;
		vec3 up(0.0f, 1.0f, 0.0f);
		float theta = fov * 3.14159265359 / 180.0f;
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

		//float x_ = (2 * (x + 0.5) / (float)xres - 1) * aspect * scale;
		//float y_ = (1 - 2 * (y + 0.5) / (float)yres) * scale;
	}

	ray get_ray(float s, float t) 
	{
		return ray(origin, lower_left_corner + h * s + v * t - origin);
	}

public:
	vec3 origin;
	vec3 lower_left_corner;
	vec3 h, v;


};