#include "light.h"


// point light functions
vec3 Point_Light::return_light_throughput(const float& d)
{
	return intensity * color / (d * d);
}

vec3 Point_Light::return_ray_direction(const vec3& hit_position, const vec2& sample)
{
	const vec3 dir = position - hit_position;
	return dir.normalize();
}


// distant light functions
vec3 Distant_Light::return_light_throughput(const float& d)
{
	return intensity * color;
}

vec3 Distant_Light::return_ray_direction(const vec3& hit_position, const vec2& sample)
{
	const vec3 min_orientation = orientation * -1.0f;
	const vec3 position = min_orientation * 100.0f;
	vec3 up(0, 1, 0);

	if (dot(up, orientation) > 0.9f || dot(up, orientation) < -0.9f) up = vec3(1, 0, 0);

	float random_angle = sample.x * 2.0f * M_PI;
	vec3 z = cross(min_orientation, up);
	vec3 y = cross(min_orientation, z);
	vec3 rand_pos = z * sample.y * angle * cos(random_angle) + y * sample.y * angle * sin(random_angle) + position;

	const vec3 dir = rand_pos - hit_position;
	return dir.normalize();
}


// square light functions
vec3 Square_Light::return_light_throughput(const float& d)
{
	return intensity * color / (d * d);
}

vec3 Square_Light::return_ray_direction(const vec3& hit_position, const vec2& sample)
{
	return transform(vec3((sample.x - 0.5f) * size.x, (sample.y - 0.5f) * size.y, 0.0f), transform_mat);
}

void Square_Light::update_positions()
{
	positions[0] = vec3(size.x / 2.0f, size.y / 2.0f, 0.0f);
	positions[1] = vec3(-size.x / 2.0f, size.y / 2.0f, 0.0f);
	positions[2] = vec3(-size.x / 2.0f, -size.y / 2.0f, 0.0f);
	positions[3] = vec3(size.x / 2.0f, -size.y / 2.0f, 0.0f);
}

void Square_Light::set_transform()
{
	mat44 translate_matrix = mat44();
	mat44 rotate_matrix = mat44();

	set_translation(translate_matrix, translate);
	set_rotation(rotate_matrix, rotate);

	transform_mat = translate_matrix * rotate_matrix;

	normal = transform_dir(vec3(0.0f, 0.0f, 1.0f), transform_mat);

	update_positions();

	for (int i = 0; i < 4; i++)
	{
		positions[i] = transform(positions[i], transform_mat);
	}
}

RTCGeometry Square_Light::set_light_geometry(RTCDevice& g_device)
{
	RTCGeometry light_geo = rtcNewGeometry(g_device, RTC_GEOMETRY_TYPE_TRIANGLE);

	rVertex* vertices = (rVertex*)rtcSetNewGeometryBuffer(light_geo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(rVertex), sizeof(positions) / sizeof(vec3));

	for (int i = 0; i < 4; i++)
	{
		vertices[i].x = positions[i].x;
		vertices[i].y = positions[i].y;
		vertices[i].z = positions[i].z;
	}

	Triangle* triangles = (Triangle*)rtcSetNewGeometryBuffer(light_geo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangle), 2);

	triangles[0].v0 = indices[0];
	triangles[0].v1 = indices[1];
	triangles[0].v2 = indices[2];
	triangles[1].v0 = indices[3];
	triangles[1].v1 = indices[4];
	triangles[1].v2 = indices[5];

	return light_geo;
}


// dome light functions
vec3 Dome_Light::return_light_throughput(const float& d)
{
	return intensity * color;
}

vec3 Dome_Light::return_ray_direction(const vec3& hit_normal, const vec2& sample)
{
	return sample_ray_in_hemisphere(hit_normal, sample);
}