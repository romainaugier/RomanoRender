#pragma once


#include "utils/vec3.h"
#include "utils/maths_utils.h"
#include "utils/matrix.h"
#include "utils/sampling_utils.h"


#ifndef LIGHT
#define LIGHT


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
	std::string name;
	vec3 color = vec3(1.0f);
	float intensity = 1.0f;

	virtual ~Light() {}

	virtual vec3 return_ray_direction(const vec3& hit_position, const vec2& sample) = 0;
	virtual vec3 return_light_throughput(const float& d) = 0;
};


// point light
class Point_Light : public Light
{
public:
	vec3 position = vec3(0.0f);

	Point_Light() { name = "Point Light"; }

	Point_Light(vec3 position, float _intensity, vec3 _color) :
		position(position)
	{
		color = _color;
		intensity = _intensity;
	}

	vec3 return_light_throughput(const float& d) override;

	vec3 return_ray_direction(const vec3& hit_position, const vec2& sample = vec2(0.0f)) override;
};


// distant light
class Distant_Light : public Light
{
public:
	vec3 orientation = vec3(0.0f, 0.0f, 1.0f);
	float angle = 1.0f;

	Distant_Light() { name = "Distant Light"; }

	Distant_Light(vec3 orientation, vec3 _color, float _intensity, float angle) : 
		orientation(orientation),
		angle(angle)
	{
		color = _color;
		intensity = _intensity;
	}

	vec3 return_light_throughput(const float& d = 0.0f) override;

	vec3 return_ray_direction(const vec3& hit_position, const vec2& sample) override;
};


// square light
class Square_Light : public Light
{
public:
	vec3 normal = vec3(0.0f, 0.0f, 1.0f);
	vec3 translate = vec3(0.0f);
	vec3 rotate = vec3(0.0f);
	mat44 transform_mat = mat44();
	vec2 size = vec2(1.0f, 1.0f);

	Square_Light() { name = "Square Light"; }

	Square_Light(vec3 _color, float _intensity)
	{
		color = _color;
		intensity = _intensity;
	}

	vec3 return_light_throughput(const float& d) override;

	vec3 return_ray_direction(const vec3& hit_position, const vec2& sample) override;

	void set_transform();
};


// dome light
class Dome_Light : public Light
{
public:
	bool visible = true;

	Dome_Light() { name = "Dome Light"; }

	Dome_Light(vec3 _color, float _intensity)
	{
		color = _color;
		intensity = _intensity;
	}

	vec3 return_light_throughput(const float& d = 0.0f) override;

	vec3 return_ray_direction(const vec3& hit_normal, const vec2& sample) override;

	vec3 return_hdri_background();
};


#endif