#pragma once
#include "vec3.h"

class light
{
public:
	float intensity;
	vec3 color;
	int temperature;
	vec3 position;
public:
	light() {}
	light(float int_, vec3 col) :
		intensity(int_),
		color(col)
	{}

	vec3 light_intensity(float d);
};


class ambient_light : public light
{
public:
	ambient_light() {}
	ambient_light(float _int, vec3 col)
	{
		intensity = _int;
		color = col;
	}
};


class point_light : public light
{
public:
	point_light() {}
	point_light(float _int, vec3 col, vec3 pos) 
	{
		intensity = _int;
		color = col;
		position = pos;
	}

	vec3 light_intensity(float d)
	{
		return intensity * color / (4 * M_PI * (d * d));
		/*
		
		*/

	}

};


class directionnal_light : public light
{
public:
	directionnal_light() {}
	directionnal_light(float _int, vec3 col, vec3 dir) :
	direction(dir)
	{
		intensity = _int;
		color = col;
	}

public:
	vec3 direction;
};