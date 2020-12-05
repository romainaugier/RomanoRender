#pragma once
#include "vec3.h"
#include "OpenImageIO/imagebuf.h"
#include "utils.h"
#include "math.h"

#ifndef LIGHT
#define LIGHT


/*
light types :
- point light
- directionnal light
- square light
- ambient light
- spot light
*/


class light
{
public:
	int type;
	float intensity;
	vec3 color;
	int temperature;
	vec3 position;

	// directional light
	vec3 direction;
	float angle;

	// square lights
	bool visible;
	float size_x;
	float size_y;
	vec3 orientation;
	vec3 v0, v1, v2, v3;
	vec3 min, max;

	// ambien light
	OIIO::ImageBuf hdri_map;
	bool has_map;
	int xres;
	int yres;


public:
	light() {}

	light(int _type, float int_, const char* file) :
		type(_type),
		intensity(int_)
	{
		hdri_map = OIIO::ImageBuf(file);
		xres = hdri_map.oriented_full_width();
		yres = hdri_map.oriented_full_height();
		has_map = true;
	}

	light(int _type, float int_, vec3 col, vec3 pos) :
		type(_type),
		intensity(int_),
		color(col),
		position(pos),
		direction(pos)
	{
		has_map = false;
	}

	light(int _type, float int_, vec3 col, vec3 dir, float angle) :
		type(_type),
		intensity(int_),
		color(col),
		direction(dir),
		angle(angle)
	{}

	light(int _type, bool v, float int_, vec3 col, vec3 pos, float sizex, float sizey, vec3 o) :
		type(_type),
		visible(v),
		intensity(int_),
		color(col),
		position(pos),
		orientation(o),
		size_x(sizex),
		size_y(sizey)
	{
		v0 = position;

		vec3 up(0, 1, 0);
		float d = dot(o, up);
		if (d > 0.9f | d < -0.9f) up = vec3(1, 0, 0);

		v1 = v0 + cross(o, up).normalize() * sizex;
		v2 = v0 - cross(o, cross(o, up)).normalize() * sizey;
		v3 = v1 + v2 - v0;

		vec3 positions[] = { v0, v1, v2, v3 };
		min = vec3(infinity);
		max = vec3(-infinity);

		for (int i = 0; i < 4; i++)
		{
			if (positions[i].x < min.x) min.x = positions[i].x;
			if (positions[i].x > max.x) max.x = positions[i].x;
			if (positions[i].y < min.y) min.y = positions[i].y;
			if (positions[i].y > max.y) max.y = positions[i].y;
			if (positions[i].z < min.z) min.z = positions[i].z;
			if (positions[i].z > max.z) max.z = positions[i].z;
		}
	}

	vec3 point_light_intensity(float d)
	{
		return intensity * color / (d * d);
	}
};


vec3 return_raydir(light& light, vec3& hit_pos, vec3& hit_normal, vec3& area_sample_position)
{
	vec3 dir(0.0f);

	if (light.type == 0)
	{
		dir = light.position - hit_pos;
	}

	if (light.type == 1)
	{
		vec3 position = -light.direction * 100.0f;
		vec3 up(0, 1, 0);

		if (dot(up, light.direction) > 0.9f | dot(up, light.direction) < -0.9f) up = vec3(1, 0, 0);
    
		float random_angle = generate_random_float() * 2.0f * M_PI;
		vec3 z = cross(-light.direction, up);
		vec3 y = cross(-light.direction, z);
		vec3 rand_pos = z * light.angle * cos(random_angle) + y * light.angle * sin(random_angle) + position;

		dir = rand_pos - hit_pos;
		dir = dir.normalize();
	}

	if (light.type == 2)
	{
		float w0 = generate_random_float();
		float w1 = generate_random_float();
		float w2 = generate_random_float();
		//float w3 = generate_random_float();

		vec3 position(0.0f);

		//dir = vec3(lerp(light.v0, light.v1, w0) + lerp(light.v1, light.v2, w1) + lerp(light.v3, light.v2, w1) + lerp(light.v0, light.v3, w0)) / 4;
		//dir = vec3(lerp(light.v0, light.v1, w0) + lerp(light.v0, light.v3, w1));
		dir = vec3(fit01(w0, light.min.x, light.max.x), fit01(w1, light.min.y, light.max.y), fit01(w2, light.min.z, light.max.z));

		area_sample_position = dir;


		dir = dir - hit_pos;
		dir = dir.normalize();
	}

	if (light.type == 3)
	{
		double r1 = generate_random_float();
		double r2 = generate_random_float();

		vec3 rand_dir_local(cos(2 * M_PI * r1) * sqrt(1 - r2), sin(2 * M_PI * r1) * sqrt(1 - r2), sqrt(1 - r1));
		vec3 rand(generate_random_float() - 0.5, generate_random_float() - 0.5, generate_random_float() - 0.5);

		vec3 tan1 = cross(hit_normal, rand);
		vec3 tan2 = cross(tan1.normalize(), hit_normal);

		dir = rand_dir_local.z * hit_normal + rand_dir_local.x * tan1 + rand_dir_local.y * tan2;
	}

	return dir;
}


vec3 return_light_int(light& light, vec3& raydir, float& d)
{
	if (light.type == 0)
	{
		return light.point_light_intensity(d);
	}

	if (light.type == 2)
	{
		return light.intensity * light.color / (d * d);
	}

	if (light.type == 3)
	{
		if (light.has_map)
		{
			vec3 invtan = vec3(0.1591, 0.3183, 0.0);
			vec3 uv = vec3(atan2(raydir.z, raydir.x), asin(raydir.y), 0.0f);
			uv = uv * invtan;
			uv += 0.5f;

			int x = light.xres * uv.x;
			int y = light.yres * uv.y;

			float pixel[3];
			light.hdri_map.OIIO::ImageBuf::getpixel(light.xres - x, light.yres - y, 0, pixel);

			vec3 color = vec3(pixel[0], pixel[1], pixel[2]);

			return color * light.intensity;
		}
		else
		{
			return light.color * light.intensity;
		}
	}

	else
	{
		return light.intensity * light.color;
	}
}


#endif