#pragma once

#include "math.h"


float oren_nayar(float sigma, float R, vec3& wo, vec3& wi)
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

	return R * inv_pi * (A + B * maxCos * sinAlpha * tanBeta);
}


float BeckmannNormalDistribution();


float GGXNormalDistribution(float& NdotH, float& roughness)
{
	float a2 = roughness * roughness;
	float d = ((NdotH * a2 - NdotH) * NdotH + 1.0f);
	return a2 / (d * d * M_PI);
}


float SchlickMaskingTerm(float& NdotL, float& NdotV, float& roughness)
{
	float k = roughness * roughness / 2.0f;

	float g_v = NdotV / (NdotV * (1.0f - k) + k);
	float g_l = NdotL / (NdotL * (1.0f - k) + k);
	return g_v * g_l;
}


vec3 SchlickFresnel(vec3& f0, float LdotH)
{
	return f0 + (vec3(1.0f) - f0) * pow(1.0f - LdotH, 5.0f);
}


vec3 GGXMicrofacet(vec3& hit_normal, float& roughness)
{
	float r0 = generate_random_float();
	float r1 = generate_random_float();

	vec3 up(0.0f, 1.0f, 0.0f);
	if (dot(up, hit_normal) > 0.9f) up = vec3(0.0f, 0.0f, 1.0f);

	vec3 b = cross(hit_normal, up).normalize();
	vec3 t = cross(b, hit_normal);
	
	float a2 = roughness * roughness;
	float cosThetaH = sqrt(std::max(0.0f, (1.0f - r0) / ((a2 - 1.0f) * r0 + 1.0f)));
	float sinThetaH = sqrt(std::max(0.0f, 1.0f - cosThetaH * cosThetaH));
	float phiH = r1 * M_PI * 2.0f;

	return t * (sinThetaH * cos(phiH)) + b * (sinThetaH * sin(phiH)) + hit_normal * cosThetaH;
}