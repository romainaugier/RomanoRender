#pragma once


#include "utils/vec3.h"
#include "OpenImageIO/imagebuf.h"
#include <OpenImageIO/imageio.h>
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
	Light_Type type = Light_Type::Point;

	virtual vec3 return_ray_direction(const vec3& hit_position, const vec2& sample) const = 0;
	virtual vec3 return_light_throughput(const float& d) const = 0;
};


// point light
class Point_Light : Light
{
public:
	vec3 position;
	vec3 color;
	float intensity;

	Light_Type type = Light_Type::Point;

	Point_Light(vec3 position, float intensity, vec3 color) :
		position(position),
		intensity(intensity),
		color(color)
	{}

	vec3 return_light_throughput(const float& d) const override;

	vec3 return_ray_direction(const vec3& hit_position, const vec2& sample = vec2(0.0f)) const override;
};


// distant light
class Distant_Light : Light
{
public:
	vec3 orientation;
	vec3 color;
	float intensity;
	float angle;

	Light_Type type = Light_Type::Distant;

	Distant_Light(vec3 orientation, vec3 color, float intensity, float angle) : 
		orientation(orientation),
		color(color),
		intensity(intensity),
		angle(angle)
	{}

	vec3 return_light_throughput(const float& d = 0.0f) const override;

	vec3 return_ray_direction(const vec3& hit_position, const vec2& sample) const override;
};


// square light
class Square_Light : Light
{
public:
	vec3 color;
	float intensity;
	mat44 transform_mat;
	vec2 size;

	Light_Type type = Light_Type::Square;

	vec3 return_light_throughput(const float& d) const override;

	vec3 return_ray_direction(const vec3& hit_position, const vec2& sample) const override;
};


// dome light
class Dome_Light : Light
{
public:
	vec3 color;
	float intensity;

	Light_Type type = Light_Type::Dome;

	vec3 return_light_throughput(const float& d = 0.0f) const override;

	vec3 return_ray_direction(const vec3& hit_normal, const vec2& sample) const override;

	vec3 return_hdri_background() const;
};


#endif