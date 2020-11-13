#pragma once
#include "vec3.h"

/*
light types :
- point light
- directionnal light
- area light
- ambient light
- spot light
*/

struct light
{
public:
	int type;
	float intensity;
	vec3 color;
	int temperature;
	vec3 position;
	vec3 direction;
public:
	light() {}

	light(int _type, float int_, vec3 col, vec3 pos) :
		type(_type),
		intensity(int_),
		color(col),
		position(pos),
		direction(pos)
		{}

	/*light(int _type, float int_, vec3 col, vec3 dir) :
		type(_type),
		intensity(int_),
		color(col),
		direction(dir) {}
	*/
	vec3 point_light_intensity(float d)
	{
		return intensity * color / (4 * M_PI * (d * d));
	}
};


vec3 return_raydir(light& light, vec3& hit_pos)
{
	vec3 dir(0.0f);

	if (light.type == 0)
	{
		dir = light.position - hit_pos;
	}

	if (light.type == 1)
	{
		dir = -light.direction;
	}

	return dir;
}


vec3 return_light_int(light& light, float& d)
{
	if (light.type == 0)
	{
		return light.point_light_intensity(d);
	}

	else
	{
		return light.intensity * light.color;
	}
}