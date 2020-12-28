#pragma once
#include "utils/vec3.h"
#include "OpenImageIO/imagebuf.h"
#include <OpenImageIO/imageio.h>
#include "utils/utils.h"
#include "utils/math.h"
#include "render/sampler.h"
#include "utils/tiles.h"

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
	vec2 area_size;
	vec3 orientation;
	vec3 v0, v1, v2, v3;
	vec3 min, max;

	// ambient light
	OIIO::ImageBuf hdri_map;
	bool has_map;
	int xres;
	int yres;
	vec3* directions;
	vec3* colors;
	float* lums;
	int size;


public:
	light() {}

	light(int _type, float int_, const char* file, bool vis) :
		type(_type),
		intensity(int_),
		visible(vis)
	{
		hdri_map = OIIO::ImageBuf(file);

		xres = hdri_map.oriented_full_width();
		yres = hdri_map.oriented_full_height();

		std::vector<vec3> important_directions;
		std::vector<vec3> important_colors;
		std::vector<float> important_lums;


		// compute average luminance
		float avg_lum = 0;
		float max_lum = 0;
		float min_lum = 100000.0f;

		for (OIIO::ImageBuf::ConstIterator<float> it(hdri_map); !it.done(); ++it)
		{
			float lum = (0.2126 * it[0] + 0.7152 * it[1] + 0.0722 * it[2]);
			avg_lum += lum / (xres * yres);
			if (lum > max_lum) max_lum = lum;
			if (lum < min_lum) min_lum = lum;
		}

		
		for (OIIO::ImageBuf::ConstIterator<float> it(hdri_map); !it.done(); ++it)
		{
			float lum = (0.2126 * it[0] + 0.7152 * it[1] + 0.0722 * it[2]);
			
			if (lum > 1.0f)
			{
				vec2 uv(((float)xres - (float)it.x()) / (float)xres, ((float)yres - (float)it.y()) / (float)yres);
				vec3 dir = toPolar(uv);
				important_directions.push_back(dir);
				important_colors.push_back(vec3(it[0], it[1], it[2]));
				important_lums.push_back(lum);
			}
			color += vec3(it[0], it[1], it[2]) / hdri_map.spec().image_pixels();
		}
		
		size = important_directions.size();
		directions = new vec3[size];
		colors = new vec3[size];
		lums = new float[size];


		int i = 0;

		for (std::vector<vec3>::iterator it = important_directions.begin(); it != important_directions.end(); ++it)
		{
			directions[i] = *it;
			i++;
		}

		i = 0;

		for (std::vector<vec3>::iterator it = important_colors.begin(); it != important_colors.end(); ++it)
		{
			colors[i] = *it;
			i++;
		}

		i = 0;

		for (std::vector<float>::iterator it = important_lums.begin(); it != important_lums.end(); ++it)
		{
			lums[i] = *it;
			i++;
		}

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
	{
	}

	light(int _type, bool v, float int_, vec3 col, vec3 pos, float sizex, float sizey, vec3 o) :
		type(_type),
		visible(v),
		intensity(int_),
		color(col),
		position(pos),
		orientation(o),
		area_size(sizex, sizey)
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


vec3 return_raydir(int& s, int& id, std::vector<vec2>& sampler, light& light, vec3& hit_pos, vec3& hit_normal, vec3& area_sample_position)
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
    
		float random_angle = sampler[s].x * 2.0f * M_PI;
		vec3 z = cross(-light.direction, up);
		vec3 y = cross(-light.direction, z);
		vec3 rand_pos = z * sampler[s].y * light.angle * cos(random_angle) + y * sampler[s].y * light.angle * sin(random_angle) + position;

		dir = rand_pos - hit_pos;
		dir = dir.normalize();
	}

	if (light.type == 2)
	{
		float w0 = generate_random_float();
		float w1 = generate_random_float();
		float w2 = generate_random_float();

		//dir = vec3(lerp(light.v0, light.v1, w0) + lerp(light.v1, light.v2, w1) + lerp(light.v3, light.v2, w1) + lerp(light.v0, light.v3, w0)) / 4;
		//dir = vec3(lerp(light.v0, light.v1, w0) + lerp(light.v0, light.v3, w1));
		dir = vec3(fit01(w0, light.min.x, light.max.x), fit01(w1, light.min.y, light.max.y), fit01(w2, light.min.z, light.max.z));

		area_sample_position = dir;

		dir = dir - hit_pos;
		dir = dir.normalize();
	}

	if (light.type == 3)
	{
		id = generate_random_float() * (light.size - 1);
		dir = light.directions[id];
	}

	return dir;
}


vec3 return_light_int(light& light, vec3& raydir, float& d, int& id)
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
		if (light.has_map && id > -1)
		{
			return light.color * HableToneMap(light.colors[id])  * clamp(light.lums[id], 0.0f, 1000.0f) * light.intensity;
		}
		else if (light.has_map)
		{
			vec3 invtan = vec3(0.1591, 0.3183, 0.0);
			vec3 uv = vec3(ApproxAtan2(raydir.z, raydir.x), asin(raydir.y), 0.0f);
			uv = uv * invtan;
			uv += 0.5f;

			int x = light.xres * uv.x;
			int y = light.yres * uv.y;

			float pixels[3];

			light.hdri_map.interppixel(light.xres - x, light.yres - y, pixels);

			vec3 color(pixels[0], pixels[1], pixels[2]);

			float luminance = (0.2126 * pixels[0] + 0.7152 * pixels[1] + 0.0722 * pixels[2]);

			return HableToneMap(color) * clamp(luminance, 0.0f, 10.0f) * light.intensity;
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