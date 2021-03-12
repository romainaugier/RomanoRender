#include "bsdf.h"


float oren_nayar(const float sigma, const float R, const vec3& wo, const vec3& wi)
{
	//sigma = deg2rad(sigma);
	float sigma2 = sigma * sigma;
	float A = 1.0f - (sigma2 / (2.f * (sigma2 + 0.33f)));
	float B = 0.45f * sigma2 / (sigma2 + 0.09f);

	float sinThetaI = SinTheta(wi);
	float sinThetaO = SinTheta(wo);

	float maxCos = 0;

	if (sinThetaI > 1e-4 && sinThetaO > 1e-4)
	{
		float sinPhiI = SinPhi(wi), cosPhiI = CosPhi(wi);
		float sinPhiO = SinPhi(wo), cosPhiO = CosPhi(wo);
		float dCos = cosPhiI * cosPhiO + sinPhiI * sinPhiO;
		maxCos = std::max(0.0f, dCos);
	}

	float sinAlpha, tanBeta;
	if (AbsCosTheta(wi) > AbsCosTheta(wo))
	{
		sinAlpha = sinThetaO;
		tanBeta = sinThetaI / AbsCosTheta(wi);
	}
	else
	{
		sinAlpha = sinThetaI;
		tanBeta = sinThetaO / AbsCosTheta(wo);
	}

	return R * M_1_PI * (A + B * maxCos * sinAlpha * tanBeta);
}


float ggx_normal_distribution(const float& NdotH, const float& roughness)
{
	float a2 = roughness * roughness;
	float d = ((NdotH * a2 - NdotH) * NdotH + 1.0f);
	return a2 / (d * d * M_PI);
}


float schlick_masking_term(const float& NdotL, const float& NdotV, const float& roughness)
{
	float k = roughness * roughness / 2.0f;

	float g_v = NdotV / (NdotV * (1.0f - k) + k);
	float g_l = NdotL / (NdotL * (1.0f - k) + k);
	return g_v * g_l;
}


vec3 schlick_fresnel(const vec3& f0, const float& LdotH)
{
	return f0 + (vec3(1.0f) - f0) * pow(1.0f - LdotH, 5.0f);
}


vec3 ggx_microfacet(const vec3& hit_normal, const float& roughness, const vec2& sample)
{
	float r0 = sample.x;
	float r1 = sample.y;

	vec3 up(0.0f, 1.0f, 0.0f);
	if (dot(up, hit_normal) > 0.9f) up = vec3(1.0f, 0.0f, 0.0f);

	vec3 b = cross(hit_normal, up);
	vec3 t = cross(b, hit_normal);

	float a2 = roughness * roughness;
	float cosThetaH = sqrt(std::max(0.0f, (1.0f - r0) / ((a2 - 1.0f) * r0 + 1.0f)));
	float sinThetaH = sqrt(std::max(0.0f, 1.0f - cosThetaH * cosThetaH));
	float phiH = r1 * M_PI * 2.0f;

	return t * (sinThetaH * cos(phiH)) + b * (sinThetaH * sin(phiH)) + hit_normal * cosThetaH;
}
