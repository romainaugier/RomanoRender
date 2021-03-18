#include "camera.h"


Ray Camera::get_ray(float s, float t)
{
	return Ray(origin, lower_left_corner + h * s + v * t - origin);
}

void Camera::update(int& xres, int& yres)
{
	aspect = (float)xres / (float)yres;

	vec3 u, w;
	vec3 up(0.0f, 1.0f, 0.0f);

	fov = 2 * rad2deg(std::atan(36.0f / (2 * focal_length)));

	float theta = fov * M_PI / 180.0f;
	float half_height = tan(theta / 2.0f);
	float half_width = aspect * half_height;

	origin = pos;

	vec3 w_(pos - rotation);
	w = w_.normalize();
	u = cross(w, up).normalize();
	v = cross(w, u);

	lower_left_corner = origin - u * half_width - v * half_height - w;
	h = u * 2.0f * half_width;
	v = v * 2.0F * half_height;
}

void Camera::set_transform()
{
	mat44 translate_matrix = mat44();
	mat44 rotate_matrix = mat44();

	set_translation(translate_matrix, pos);
	set_rotation(rotate_matrix, rotation);

	transformation_matrix = translate_matrix * rotate_matrix;
}