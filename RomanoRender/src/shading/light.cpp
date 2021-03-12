#include "light.h"


// point light functions
vec3 Point_Light::return_light_throughput(const float& d) const
{
	return intensity * color / (d * d);
}

vec3 Point_Light::return_ray_direction(const vec3& hit_position, const vec2& sample = vec2(0.0f)) const
{
	const vec3 dir = position - hit_position;
	return dir.normalize();
}


// distant light functions
vec3 Distant_Light::return_light_throughput(const float& d = 0.0f) const
{
	return intensity * color;
}

vec3 Distant_Light::return_ray_direction(const vec3& hit_position, const vec2& sample) const
{
	const vec3 min_orientation = orientation * -1.0f;
	const vec3 position = min_orientation * 100.0f;
	vec3 up(0, 1, 0);

	if (dot(up, orientation) > 0.9f | dot(up, orientation) < -0.9f) up = vec3(1, 0, 0);

	float random_angle = sample.x * 2.0f * M_PI;
	vec3 z = cross(min_orientation, up);
	vec3 y = cross(min_orientation, z);
	vec3 rand_pos = z * sample.y * angle * cos(random_angle) + y * sample.y * angle * sin(random_angle) + position;

	const vec3 dir = rand_pos - hit_position;
	return dir.normalize();
}


// square light functions
vec3 Square_Light::return_light_throughput(const float& d) const
{
	return intensity * color / (d * d);
}

vec3 Square_Light::return_ray_direction(const vec3& hit_position, const vec2& sample) const
{
	const vec3 light_sample_position = transform(vec3(sample.x - 0.5f, sample.y - 0.5f, 0.0f), transform_mat);
	const vec3 dir = light_sample_position - hit_position;
	return dir.normalize();
}


// dome light functions
vec3 Dome_Light::return_light_throughput(const float& d = 0.0f) const
{
	return intensity * color;
}

vec3 Dome_Light::return_ray_direction(const vec3& hit_normal, const vec2& sample) const 
{
	return sample_ray_in_hemisphere(hit_normal, sample);
}