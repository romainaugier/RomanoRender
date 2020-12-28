#pragma once
#include "vec3.h"
#include "utils.h"


void createBasis(vec3& hit_normal, vec3& tangent, vec3& bitangent)
{
	vec3 up(0.0f, 1.0f, 0.0f);
	if (dot(up, hit_normal) > 0.9f) up = vec3(0.0f, 0.0f, 1.0f);

	tangent = cross(hit_normal, up).normalize();
	bitangent = cross(hit_normal, tangent);
}


void worldToTangent(vec3& hit_normal, vec3& tangent, vec3& bitangent, vec3& l, vec3& v, vec3& wo, vec3& wi, vec3& wm)
{
	wo = (v.z * hit_normal + v.x * tangent + v.y * bitangent).normalize();
	wi = (l.z * hit_normal + l.x * tangent + v.y * bitangent).normalize();
	wm = (wo + wi).normalize();
}


vec3 tangentToWorld(vec3& hit_normal, vec3& tangent, vec3& bitangent, vec3& t)
{
	vec3 wdir;
	wdir = tangent * t.x + hit_normal * t.y + bitangent * t.z;
	return wdir;
}


inline float Log2(float x)
{
	const float invLog2 = 1.442695040888963387004650940071;
	return std::log(x) * invLog2;
}


inline float square(float x) { return x * x; }


inline float Saturate(float x)
{
	if (x < 0.0f) {
		return 0.0f;
	}
	else if (x > 1.0f) {
		return 1.0f;
	}

	return x;
}


static float inv_pi = 0.31830988618379067154;


// trig identities
inline float CosTheta(vec3& w) { return w.z; }
inline float Cos2Theta(vec3& w) { return w.z * w.z; }
inline float AbsCosTheta(vec3& w) { return std::abs(w.z); }
inline float Sin2Theta(vec3& w) { return std::max(0.0f, 1.0f - Cos2Theta(w)); }
inline float SinTheta(vec3& w) { return sqrt(Sin2Theta(w)); }
inline float TanTheta(vec3& w) { return SinTheta(w) / CosTheta(w); }
inline float Tan2Theta(vec3& w) { return Sin2Theta(w) / Cos2Theta(w); }
inline float CosPhi(vec3& w)
{
	float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 1 : clamp(w.x / sinTheta, -1.0f, 1.0f);
}
inline float SinPhi(vec3& w)
{
	float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 0 : clamp(w.y / sinTheta, -1.0f, 1.0f);
}
inline float Cos2Phi(vec3& w) { return CosPhi(w) * CosPhi(w); }
inline float Sin2Phi(vec3& w) { return SinPhi(w) * SinPhi(w); }
inline float CosDPhi(vec3& wa, vec3& wb) { return clamp((wa.x * wb.x + wa.y * wb.y) / sqrt((wa.x * wa.x + wa.y * wa.y) * (wb.x * wb.x + wb.y * wb.y)), -1.0f, 1.0f); }


// Schlicks/Fresnels
inline vec3 SchlickWeight(vec3& f0, float& h) { return f0 + (1.0f - f0) * std::pow(1 - h, 5); }
inline float SchlickR0FromRelativeIOR(float eta) { return square(eta - 1.0f) / square(eta + 1.0f); }


float FresnelReflectionCoef(float& n2, vec3& normal, vec3& incident)
{
	float n1 = 1.0f;
	float r0 = (n1 - n2) / (n1 + n2);
	float cosX = -dot(normal, incident);

	if (n1 > n2)
	{
		float n = n1 / n2;
		float sint2 = n * n * (1.0f - cosX * cosX);

		if (sint2 > 0.9999f) return 1.0f;
		cosX = sqrt(1.0f - sint2);
	}
	float x = 1.0f - cosX;
	return r0 + (1.0f - r0) * x * x * x * x * x;
}


float RoughnessToAlpha(float roughness) {
	roughness = std::max(roughness, (float)1e-3);
	float x = std::log(roughness);
	return 1.62142f + 0.819955f * x + 0.1734f * x * x +
		0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
}


inline float fit01(float x, float a, float b) { return x * (b - a) + a; }


inline float fit(float s, float a1, float a2, float b1, float b2)
{
	return b1 + ((s - a1) * (b2 - b1)) / (a2 - a1);
}


inline float PowerHeuristic(int nf, float fpdf, int ng, float gpdf)
{
	float f = nf * fpdf;
	float g = ng * gpdf;
	return (f * f) / (f * f + g * g);
}


inline float ApproxAtan(float z)
{
	const float n1 = 0.97239411f;
	const float n2 = -0.19194795f;
	return (n1 + n2 * z * z) * z;
}


float ApproxAtan2(float y, float x)
{
	const float n1 = 0.97239411f;
	const float n2 = -0.19194795f;
	float result = 0.0f;
	if(x != 0.0f)
	{
		const union { float flVal; int nVal; } tYSign = { y };
		const union { float flVal; int nVal; } tXSign = { x };
		if (fabsf(x) >= fabsf(y))
		{
			union { float flVal; int nVal; } tOffset = { M_PI };
			tOffset.nVal *= tYSign.nVal & 0x80000000u;
			tOffset.nVal *= tXSign.nVal >> 31;
			result = tOffset.flVal;
			const float z = y / x;
			result += (n1 + n2 * z * z) * z;
		}
		else
		{
			union { float flVal; int nVal; } tOffset = { M_PI_2 };
			tOffset.nVal |= tYSign.nVal & 0x80000000u;
			result = tOffset.flVal;
			const float z = x / y;
			result -= (n1 + n2 * z * z) * z;
		}
	}
	else if (y > 0.0f)
	{
		result = M_PI_2;
	}
	else if (y < 0.0f)
	{
		result = -M_PI_2;
	}
	return result;
}