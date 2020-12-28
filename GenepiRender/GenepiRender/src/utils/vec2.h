#pragma once

#ifndef VEC2
#define VEC2


struct alignas(8) vec2
{
	float x, y;

	vec2() {}
	vec2(float a, float b) :
		x(a),
		y(b) {}
};


#endif // !VEC2
