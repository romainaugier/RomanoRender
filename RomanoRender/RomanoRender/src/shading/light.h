#pragma once
#include "utils/vec3.h"
#include "OpenImageIO/imagebuf.h"
#include <OpenImageIO/imageio.h>
#include "utils/utils.h"
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


enum class Light_Type
{
	Point,
	Distant,
	Square,
	Dome
};


class Light
{
public:
	Light_Type light_type;

	int type;
	float intensity;
	Vec3 color;
	int temperature;
	Vec3 position;

	// directional light
	Vec3 direction;
	float angle;

	// square lights
	bool visible;
	vec2 area_size;
	Vec3 orientation;
	Vec3 v0, v1, v2, v3;
	Vec3 min, max;

	// ambient light
	OIIO::ImageBuf hdri_map;
	bool has_map;
	int xres;
	int yres;
	Vec3* directions;
	Vec3* colors;
	float* lums;
	int size;


public:
	Light() 
	{
		intensity = 1.0f;
		color = Vec3(1.0f);
		temperature = 0.0f;
		position = Vec3(0.0f);

		direction = Vec3(0.0f);
		angle = 0.0f;

		visible = true;
		area_size = vec2(1.0f, 1.0f);
		orientation = Vec3(0.0f);

		has_map = false;
	}

	Light(Light_Type type, float int_, char* file, bool vis) :
		light_type(type),
		intensity(int_),
		visible(vis) {}

	Light(Light_Type type, float int_, Vec3 col, Vec3 pos) :
		light_type(type),
		intensity(int_),
		color(col),
		position(pos),
		direction(pos)
	{
		has_map = false;
	}

	Light(Light_Type type, float int_, Vec3 col, Vec3 dir, float angle) :
		light_type(type),
		intensity(int_),
		color(col),
		direction(dir),
		angle(angle)
	{
	}

	Light(Light_Type type, bool v, float int_, Vec3 col, Vec3 pos, float sizex, float sizey, Vec3 o) :
		light_type(type),
		visible(v),
		intensity(int_),
		color(col),
		position(pos),
		orientation(o),
		area_size(sizex, sizey)
	{
		v0 = position;

		Vec3 up(0, 1, 0);
		float d = dot(o, up);
		if (d > 0.9f | d < -0.9f) up = Vec3(1, 0, 0);

		v1 = v0 + cross(o, up).normalize() * sizex;
		v2 = v0 - cross(o, cross(o, up)).normalize() * sizey;
		v3 = v1 + v2 - v0;

		Vec3 positions[] = { v0, v1, v2, v3 };
		min = Vec3(infinity);
		max = Vec3(-infinity);

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

	Vec3 point_light_intensity(float d)
	{
		return intensity * color / (d * d);
	}

	void InitializeHDRI(char* file)
	{
		hdri_map = OIIO::ImageBuf(file);

		xres = hdri_map.oriented_full_width();
		yres = hdri_map.oriented_full_height();

		std::vector<Vec3> important_directions;
		std::vector<Vec3> important_colors;
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

			if (lum > avg_lum)
			{
				vec2 uv(((float)xres - (float)it.x()) / (float)xres, ((float)yres - (float)it.y()) / (float)yres);
				Vec3 dir = toPolar(uv);
				important_directions.push_back(dir);
				important_colors.push_back(clamp(Vec3(it[0], it[1], it[2]), 0.0f, 1.0f));
				important_lums.push_back(lum);
			}
			color += clamp(Vec3(it[0], it[1], it[2]), 0.0f, 1.0f) / hdri_map.spec().image_pixels();
		}

		size = important_directions.size();
		directions = new Vec3[size];
		colors = new Vec3[size];
		lums = new float[size];


		int i = 0;

		for (std::vector<Vec3>::iterator it = important_directions.begin(); it != important_directions.end(); ++it)
		{
			directions[i] = *it;
			i++;
		}

		i = 0;

		for (std::vector<Vec3>::iterator it = important_colors.begin(); it != important_colors.end(); ++it)
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

	void InitializeSquare()
	{
		v0 = position;

		Vec3 up(0, 1, 0);
		float d = dot(orientation, up);
		if (d > 0.9f | d < -0.9f) up = Vec3(1, 0, 0);

		v1 = v0 + cross(orientation, up).normalize() * area_size.x;
		v2 = v0 - cross(orientation, cross(orientation, up)).normalize() * area_size.y;
		v3 = v1 + v2 - v0;

		Vec3 positions[] = { v0, v1, v2, v3 };
		min = Vec3(infinity);
		max = Vec3(-infinity);

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
};


Vec3 return_raydir(int& id, const vec2& sample, Light& light, Vec3& hit_pos, Vec3& hit_normal, Vec3& area_sample_position, const float& random_float)
{
	ZoneScoped;
	Vec3 dir(0.0f);


	if (light.light_type == Light_Type::Distant)
	{
		Vec3 position = -light.direction * 100.0f;
		Vec3 up(0, 1, 0);

		if (dot(up, light.direction) > 0.9f | dot(up, light.direction) < -0.9f) up = Vec3(1, 0, 0);
    
		float random_angle = sample.x * 2.0f * M_PI;
		Vec3 z = cross(-light.direction, up);
		Vec3 y = cross(-light.direction, z);
		Vec3 rand_pos = z * sample.y * light.angle * cos(random_angle) + y * sample.y * light.angle * sin(random_angle) + position;

		dir = rand_pos - hit_pos;
		dir = dir.normalize();
	}

	else if (light.light_type == Light_Type::Square)
	{
		float w0 = generate_random_float();
		float w1 = generate_random_float();
		float w2 = generate_random_float();

		//dir = vec3(lerp(light.v0, light.v1, w0) + lerp(light.v1, light.v2, w1) + lerp(light.v3, light.v2, w1) + lerp(light.v0, light.v3, w0)) / 4;
		//dir = vec3(lerp(light.v0, light.v1, w0) + lerp(light.v0, light.v3, w1));
		dir = Vec3(fit01(w0, light.min.x, light.max.x), fit01(w1, light.min.y, light.max.y), fit01(w2, light.min.z, light.max.z));

		area_sample_position = dir;

		dir = dir - hit_pos;
		dir = dir.normalize();
	}

	else if (light.light_type == Light_Type::Dome)
	{
		if (light.has_map)
		{
			id = (int)(random_float * (light.size - 1));
			dir = light.directions[id];

		}
		else
		{
			dir = sample_dome_light(hit_normal, sample);
		}
	}


	else
	{
		dir = light.position - hit_pos;
	}

	return dir;
}


Vec3 return_light_int(Light& light, Vec3& raydir, float& d, int& id)
{
	if (light.light_type == Light_Type::Point)
	{
		return light.point_light_intensity(d);
	}

	if (light.light_type == Light_Type::Square)
	{
		return light.intensity * light.color / (d * d);
	}

	if (light.light_type == Light_Type::Dome)
	{
		if (light.has_map && id > -1)
		{
			return light.color * light.colors[id] * clamp(light.lums[id], 0.0f, 1000.0f) * light.intensity;
		}
		else if (light.has_map)
		{
			Vec3 invtan = Vec3(0.1591, 0.3183, 0.0);
			Vec3 uv = Vec3(ApproxAtan2(raydir.z, raydir.x), asin(raydir.y), 0.0f);
			uv = uv * invtan;
			uv += 0.5f;

			int x = light.xres * uv.x;
			int y = light.yres * uv.y;

			float pixels[3];

			light.hdri_map.interppixel(light.xres - x, light.yres - y, pixels);

			Vec3 color(pixels[0], pixels[1], pixels[2]);

			float luminance = (0.2126 * pixels[0] + 0.7152 * pixels[1] + 0.0722 * pixels[2]);

			return clamp(color, 0.0f, 1.0f) * light.intensity;
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