#pragma once
#include <math.h>
#include <xmmintrin.h>

#define M_PI 3.14159265358979323846f
#define FLT_MAX 3.402823466e+38F


// Shuffle helpers.
// Examples: SHUFFLE3(v, 0,1,2) leaves the vector unchanged.
//           SHUFFLE3(v, 0,0,0) splats the X coord out.
#define SHUFFLE3(V, X,Y,Z) float3(_mm_shuffle_ps((V).m, (V).m, _MM_SHUFFLE(Z,Z,Y,X)))


struct alignas(16) float3
{
	__m128 m;

	__forceinline float3() {}
	__forceinline explicit float3(const float* p) { m = _mm_set_ps(p[2], p[2], p[1], p[0]); }
	__forceinline explicit float3(float x, float y, float z) { m = _mm_set_ps(z, z, y, x); }
	__forceinline explicit float3(__m128 v) { m = v; }

	__forceinline float x() { return _mm_cvtss_f32(m); }
	__forceinline float y() { return _mm_cvtss_f32(_mm_shuffle_ps(m, m, _MM_SHUFFLE(1, 1, 1, 1))); }
	__forceinline float z() { return _mm_cvtss_f32(_mm_shuffle_ps(m, m, _MM_SHUFFLE(2, 2, 2, 2))); }

	__forceinline float3 yzx() const 

	__forceinline void setX(float x) { m = _mm_move_ss(m, _mm_set_ss(x)); }
	__forceinline void setY(float y) 
	{
		__m128 t = _mm_move_ss(m, _mm_set_ss(y)); 
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 2, 0, 0));
		m = _mm_move_ss(t, m);
	}
	__forceinline void setZ(float z)
	{
		__m128 t = _mm_move_ss(m, _mm_set_ss(z));
		t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 0, 1, 0));
		m = _mm_move_ss(t, m);
	}

	__forceinline float operator[] (size_t i) const { return m.m128_f32[i]; }
	__forceinline float& operator[] (size_t i) { return m.m128_f32[i]; }


};