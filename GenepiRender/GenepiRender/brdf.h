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


float BeckMannDistribution(vec3& wh, float& ax, float& ay)
{
	float tan2Theta = Tan2Theta(wh);
	if (std::isinf(tan2Theta)) return 0.0f;
	float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
	return std::exp(-tan2Theta * (Cos2Phi(wh) / (ax * ax) + Sin2Phi(wh) / (ay * ay))) / (M_PI * ax * ay * cos4Theta);
}


float Lambda(vec3& w, float& ax, float& ay)
{
	float absTanTheta = abs(TanTheta(w));
	if (std::isinf(absTanTheta)) return 0.0f;
	float alpha = sqrt(Cos2Phi(w) * ax * ax + Sin2Phi(w) * ay * ay);
	float a = 1.0f / (alpha * absTanTheta);
	if (a >= 1.6f) return 0.0f;
	return (1.0F - 1.259f * a + 0.396 * a * a) / (3.535f * a + 2.181f * a * a);
}


float G1(vec3& w, float& ax, float& ay)
{
	return 1.0f / (1.0f + Lambda(w, ax, ay));
}


float G(vec3& wo, vec3& wi, float& ax, float& ay)
{
	return 1.0f / (1.0f + Lambda(wo, ax, ay) + Lambda(wi, ax, ay));
}


vec3 TorranceSparrow(vec3& wo, vec3& wi, float& ax, float& ay, vec3& f0, vec3& ks)
{
	float costhetaO = AbsCosTheta(wo), cosThetaI = AbsCosTheta(wi);
	vec3 wh = wi + wo;
	if (cosThetaI == 0.0f || costhetaO == 0.0f) return 0.0f;
	if (wh.x == 0.0f && wh.y == 0.0f && wh.z == 0.0f) return 0.0f;

	wh = wh.normalize();
	float d = dot(wo, wh);
	vec3 F = SchlickWeight(f0, d);
	ks += F;

	return BeckMannDistribution(wh, ax, ay) * G(wo, wi, ax, ay) * F / (4 * cosThetaI * costhetaO);
}