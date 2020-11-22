#pragma once
#include "vec3.h"
#include "utils.h"

#ifndef BSDF

/*
wi = incident light direction
wo = outgoing viewing direction
wm = half vector between light direction and view direction
*/

/*
// BSDF Inline Functions (PBRT Book)

// Utilities
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
inline float SchlickWeight(float f0, float& h) { return f0 + (1.0f - f0) * std::pow(1 - h, 5); }
inline float SchlickR0FromRelativeIOR(float eta) { return square(eta - 1.0f) / square(eta + 1.0f); }

float FresnelDielectric(float cosThetaI, float etaI, float etaT)
{
	cosThetaI = clamp(cosThetaI, -1, 1);
	bool entering = cosThetaI > 0.f;

	if (!entering)
	{
		std::swap(etaI, etaT);
		cosThetaI = abs(cosThetaI);
	}

	float sinThetaI = sqrt(std::max(0.1f, 1 - cosThetaI * cosThetaI));
	float sinThetaT = etaI / etaT * sinThetaI;

	if (sinThetaT >= 1) return 1.0f;

	float cosThetaT = sqrt(std::max(0.f, 1.0f - sinThetaT * sinThetaT));
	float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
	float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));

	return (Rparl * Rparl + Rperp * Rperp) / 2.0f;
}


// Sample struct
struct BsdfSample
{
	vec3 reflectance = vec3(0.0f);
	vec3 wi = vec3(0.0f);
	float forwardPdfW = 0.0f;
	float reversePdfW = 0.0f;
};



// Disney Principled Shader (https://schuttejoe.github.io/post/disneybsdf/ helped me a lot to understand and implement it)


// Sheen
vec3 CalculateTint(vec3& diffuse_color)
{
	float luminance = dot(vec3(0.3f, 0.6f, 1.0f), diffuse_color);
	return (luminance > 0.0f) ? diffuse_color * (1.0f / luminance) : vec3(1.0f);
}

vec3 EvaluateSheen(material& mat, vec3& wo, vec3& wm, vec3& wi)
{
	if (mat.sheen <= 0.0f)
	{
		return vec3(0.0f);
	}

	float HdotL = dot(wm, wi);
	vec3 tint = CalculateTint(mat.clr);
	return mat.sheen * lerp(vec3(1.0f), tint, mat.sheen) * SchlickWeight(0.04f, HdotL);
}


// ClearCoat
float GTR1(float absHdotL, float a)
{
	if (a >= 1) return inv_pi;

	float a2 = a * a;
	return (a2 - 1.0f) / (M_PI * Log2(a2) * (1.0f + (a2 - 1.0f) * absHdotL * absHdotL));
}

float SeparableSmithGGXG1(vec3& w, float a)
{
	float a2 = a * a;
	float absNdotV = AbsCosTheta(w);

	return 2.0f / (1.0f + sqrtf(a2 + (1.0f - a2) * absNdotV * absNdotV));
}

float EvaluateClearCoat(float clearcoat, float alpha, vec3& wo, vec3& wm, vec3& wi, float& fPdfW, float& rPdfW)
{
	if (clearcoat <= 0.0f) return 0.0f;

	float absNdotH = AbsCosTheta(wm);
	float absNdotL = AbsCosTheta(wi);
	float absNdotV = AbsCosTheta(wo);
	float HdotL = dot(wm, wi);

	float d = GTR1(absNdotH, lerp(0.1f, 0.001f, alpha));
	float f = SchlickWeight(0.04f, HdotL);
	float gl = SeparableSmithGGXG1(wi, 0.25f);
	float gv = SeparableSmithGGXG1(wo, 0.25f);

	fPdfW = d / (4.0f * absNdotL);
	rPdfW = d / (4.0f * absNdotV);

	return 0.25f * clearcoat * d * f * gl * gv;
}


// Specular BRDF
float GgxAnisotropicD(vec3& wm, float ax, float ay)
{
	float HdotX2 = square(wm.x);
	float HdotY2 = square(wm.z);
	float cos2Theta = Cos2Theta(wm);
	float ax2 = square(ax);
	float ay2 = square(ay);
	
	return 1.0f / (M_PI * ax * ay * square(HdotX2 / ax2 + HdotY2 / ay2 + cos2Theta));
}

float SeparableSmithGGXG1(vec3& w, vec3& wm, float ax, float ay)
{
	float HdotW = dot(w, wm);
	if (HdotW <= 0.0f) return 0.0f;

	float absTanTheta = abs(TanTheta(w));
	if (isinf(absTanTheta)) return 0.0f;

	float a = sqrtf(Cos2Phi(w) * ax * ax + Sin2Phi(w) * ay * ay);
	float a2Tan2Theta = square(a * absTanTheta);

	float lambda = 0.5f * (-1.0f + sqrtf(1.0f + a2Tan2Theta));
	return 1.0f / (1.0f + lambda);
}

void CalculateAnisotropicParams(float roughness, float anisotropic, float& ax, float& ay)
{
	float aspect = sqrtf(1.0f - 0.9f * anisotropic);
	ax = std::max(0.001f, square(roughness) / aspect);
	ay = std::max(0.001f, square(roughness) * aspect);
}

vec3 DisneyFresnel(material& mat, vec3& wo, vec3& wm, vec3& wi)
{
	float HdotV = abs(dot(wm, wo));
	vec3 tint = CalculateTint(mat.clr);

	vec3 R0 = SchlickR0FromRelativeIOR(mat.relativeIOR) * lerp(vec3(1.0f), tint, mat.specularTint);
	R0 = lerp(R0, mat.clr, mat.metallic);
}

void GgxVndfAnisotropicPdf(vec3& wi, vec3& wm, vec3& wo, float ax, float ay,
	float& forwardPdfW, float& reversePdfW)
{
	float D = GgxAnisotropicD(wm, ax, ay);

	float absDotNL = AbsCosTheta(wi);
	float absDotHL = abs(dot(wm, wi));
	float G1v = SeparableSmithGGXG1(wo, wm, ax, ay);
	forwardPdfW = G1v * absDotHL * D / absDotNL;

	float absDotNV = AbsCosTheta(wo);
	float absDotHV = abs(dot(wm, wo));
	float G1l = SeparableSmithGGXG1(wi, wm, ax, ay);
	reversePdfW = G1l * absDotHV * D / absDotNV;
}

vec3 EvalBRDF(material& mat, vec3& wo, vec3& wm, vec3& wi, float& fPdf, float& rPdf)
{
	fPdf = 0.0f;
	rPdf = 0.0f;

	float NdotL = CosTheta(wi);
	float NdotV = CosTheta(wo);
	if (NdotL <= 0.0f || NdotV <= 0.0f) return vec3(0.0f);

	float ax, ay;
	CalculateAnisotropicParams(mat.roughness, mat.anisotropic, ax, ay);

	float d = GgxAnisotropicD(wm, ax, ay);
	float gl = SeparableSmithGGXG1(wi, wm, ax, ay);
	float gv = SeparableSmithGGXG1(wo, wm, ax, ay);

	vec3 f = DisneyFresnel(mat, wo, wm, wi);

	GgxVndfAnisotropicPdf(wi, wm, wo, ax, ay, fPdf, rPdf);
	fPdf *= (1.0f / (4.0f * abs(dot(wo, wm))));
	rPdf *= (1.0f / (4.0f * abs(dot(wi, wm))));

	return d * gl * gv * f / (4.0f * NdotL * NdotV);
}


// Specular BSDF
float ThinTransmissionRoughness(float ior, float roughness)
{
	return Saturate((0.65f * ior - 0.35f) * roughness);
}

vec3 EvalSpecTransmission(material& mat, vec3& wo, vec3& wm, vec3& wi, float ax, float ay, bool thin)
{
	float relativeIor = mat.relativeIOR;
	float n2 = relativeIor * relativeIor;

	float absNdotL = AbsCosTheta(wi);
	float absNdotV = AbsCosTheta(wo);
	float HdotL = dot(wm, wi);
	float HdotV = dot(wm, wo);
	float absHdotL = abs(HdotL);
	float absHdotV = abs(HdotV);

	float d = GgxAnisotropicD(wm, ax, ay);
	float gl = SeparableSmithGGXG1(wi, wm, ax, ay);
	float gv = SeparableSmithGGXG1(wo, wm, ax, ay);

	float f = FresnelDielectric(HdotV, 1.0f, 1.0f / mat.relativeIOR);

	vec3 color;
	if (thin) color = vec3(sqrt(mat.clr.x), sqrt(mat.clr.y), sqrt(mat.clr.z));
	else color = mat.clr;

	float c = (absHdotL * absHdotV) / (absNdotL * absNdotV);
	float t = (n2 / square(HdotL + relativeIor * HdotV));
	return color * c * t * (1.0f - f) * gl * gv * d;
}


// Diffuse
float EvaluateRetroDiffuse(material& mat, vec3& wo, vec3& wm, vec3& wi)
{
	float dotNL = AbsCosTheta(wi);
	float dotNV = AbsCosTheta(wo);

	float roughness = mat.roughness * mat.roughness;

	float rr = 0.5f + 2.0f * dotNL * dotNL * roughness;
	float fl = SchlickWeight(0.04f, dotNL);
	float fv = SchlickWeight(0.04f, dotNV);

	return rr * (fl + fv + fl * fv * (rr - 1.0f));
}

float EvalDiffuse(material& mat, vec3& wo, vec3& wm, vec3& wi, bool thin)
{
	float NdotL = AbsCosTheta(wi);
	float NdotV = AbsCosTheta(wo);

	float fl = SchlickWeight(0.04f, NdotL);
	float fv = SchlickWeight(0.04f, NdotV);

	float hanrahanKrueger = 0.0f;

	if (thin && mat.flatness > 0.0f)
	{
		float roughness = mat.roughness * mat.roughness;

		float HdotL = dot(wm, wi);
		float fss90 = HdotL * HdotL * roughness;
		float fss = lerp(1.0f, fss90, fl) * lerp(1.0f, fss90, fv);

		float ss = 1.25f * (fss * (1.0f / (NdotL + NdotV) - 0.5f) + 0.5f);
		hanrahanKrueger = ss;
	}

	float lambert = 1.0f;
	float retro = EvaluateRetroDiffuse(mat, wo, wm, wi);

	float subsurfaceApprox = lerp(lambert, hanrahanKrueger, thin ? mat.flatness : 0.0f);

	return inv_pi * (retro + subsurfaceApprox * (1.0f - 0.5f * fl) * (1.0f - 0.5f * fv));
}


// Evaluate All
static void CalculateLobePdfs(material& surface,
	float& pSpecular, float& pDiffuse, float& pClearcoat, float& pSpecTrans)
{
	float metallicBRDF = surface.metallic;
	float specularBSDF = (1.0f - surface.metallic) * surface.specTrans;
	float dielectricBRDF = (1.0f - surface.specTrans) * (1.0f - surface.metallic);

	float specularWeight = metallicBRDF + dielectricBRDF;
	float transmissionWeight = specularBSDF;
	float diffuseWeight = dielectricBRDF;
	float clearcoatWeight = 1.0f * Saturate(surface.clearcoat);

	float norm = 1.0f / (specularWeight + transmissionWeight + diffuseWeight + clearcoatWeight);

	pSpecular = specularWeight * norm;
	pSpecTrans = transmissionWeight * norm;
	pDiffuse = diffuseWeight * norm;
	pClearcoat = clearcoatWeight * norm;
}

vec3 Evaluate(material& mat, vec3& v, vec3& l, vec3& hit_normal, bool thin, float& forwardPdf, float& reversePdf)
{
	vec3 rand(generate_random_float() - 0.5, generate_random_float() - 0.5, generate_random_float() - 0.5);

	vec3 tan1 = cross(hit_normal, rand);
	vec3 tan2 = cross(tan1.normalize(), hit_normal);

	vec3 wo = (v.z * hit_normal + v.x * tan1 + v.y * tan2).normalize();
	vec3 wi = (l.z * hit_normal + l.x * tan1 + l.y * tan2).normalize();
	vec3 wm = (wo + wi).normalize();

	float NdotV = CosTheta(wo);
	float NdotL = CosTheta(wi);

	vec3 reflectance = vec3(0.0f);
	forwardPdf = 0.0f;
	reversePdf = 0.0f;

	float pBRDF, pDiffuse, pClearcoat, pSpecTrans;
	CalculateLobePdfs(mat, pBRDF, pDiffuse, pClearcoat, pSpecTrans);

	vec3 baseColor = mat.clr;
	float metallic = mat.metallic;
	float specTrans = mat.specTrans;
	float roughness = mat.roughness;

	// anisotropic parms
	float ax, ay;
	CalculateAnisotropicParams(mat.roughness, mat.anisotropic, ax, ay);

	float diffuseWeight = (1.0f - metallic) * (1.0f - specTrans);
	float transWeight = (1.0f - metallic) * specTrans;

	// Clearcoat
	bool upperHemisphere = NdotL > 0.0f && NdotV > 0.0f;
	if (upperHemisphere && mat.clearcoat > 0.0f)
	{
		float forwardClearcoatPdfW;
		float reverseClearCoatPdfW;

		float clearCoat = EvaluateClearCoat(mat.clearcoat, mat.clearcoatGloss, wo, wm, wi, forwardClearcoatPdfW, reverseClearCoatPdfW);

		reflectance += vec3(clearCoat);
		forwardPdf += pClearcoat * forwardClearcoatPdfW;
		reversePdf += pClearcoat * reverseClearCoatPdfW;
	}

	// Diffuse
	if (diffuseWeight > 0.0f)
	{
		float forwardDiffusePdfW = AbsCosTheta(wi);
		float reverseDiffusePdfW = AbsCosTheta(wo);
		float diffuse = EvalDiffuse(mat, wo, wm, wi, thin);

		vec3 sheen = EvaluateSheen(mat, wo, wm, wi);

		reflectance += diffuseWeight * (diffuse * mat.clr + sheen);

		forwardPdf += pDiffuse * forwardDiffusePdfW;
		reversePdf = pDiffuse * reverseDiffusePdfW;
	}

	// Transmission
	if (transWeight > 0.0f)
	{
		float rscaled = thin ? ThinTransmissionRoughness(mat.ior, mat.roughness) : mat.roughness;
		float tax, tay;
		CalculateAnisotropicParams(rscaled, mat.anisotropic, tax, tay);

		vec3 transmission = EvalSpecTransmission(mat, wo, wm, wi, tax, tay, thin);
		reflectance += transWeight * transmission;

		float forwardTransmissivePdfW;
		float reverseTransmissivePdfW;
		GgxVndfAnisotropicPdf(wi, wm, wo, tax, tay, forwardTransmissivePdfW, reverseTransmissivePdfW);

		float LdotH = dot(wm, wi);
		float VdotH = dot(wm, wo);
		forwardPdf += pSpecTrans * forwardTransmissivePdfW / (square(LdotH + mat.relativeIOR * VdotH));
		reversePdf += pSpecTrans * reverseTransmissivePdfW / (square(VdotH + mat.relativeIOR * LdotH));
	}

	// Specular
	if (upperHemisphere)
	{
		float forwardMetallicPdfW;
		float reverseMetallicPdfW;

		vec3 specular = EvalBRDF(mat, wo, wm, wi, forwardMetallicPdfW, reverseMetallicPdfW);

		reflectance += specular;
		forwardPdf += pBRDF * forwardMetallicPdfW / (4 * abs(dot(wo, wm)));
		reversePdf += pBRDF * reverseMetallicPdfW / (4 * abs(dot(wi, wm)));
	}

	reflectance = reflectance * abs(NdotL);

	return reflectance;
}


// Sampling
bool SampleSpecTransmission(material& mat, vec3& v, vec3& hit_normal, bool thin, BsdfSample& sample)
{
	vec3 rand(generate_random_float() - 0.5, generate_random_float() - 0.5, generate_random_float() - 0.5);

	vec3 tan1 = cross(hit_normal, rand);
	vec3 tan2 = cross(tan1.normalize(), hit_normal);

	vec3 wo = v.z * hit_normal + v.x * tan1 + v.y * tan2;

	if (CosTheta(wo) == 0.0) {
		sample.forwardPdfW = 0.0f;
		sample.reversePdfW = 0.0f;
		sample.reflectance = vec3(0.0f);
		sample.wi = vec3(0.0f);
		return false;
	}

	// -- Scale roughness based on IOR
	float rscaled = thin ? ThinTransmissionRoughness(mat.ior, mat.roughness) : mat.roughness;

	float tax, tay;
	CalculateAnisotropicParams(rscaled, mat.anisotropic, tax, tay);

	// -- Sample visible distribution of normals
	float r0 = generate_random_float();
	float r1 = generate_random_float();
	vec3 wm = SampleGgxVndfAnisotropic(wo, tax, tay, r0, r1);

	float dotVH = dot(wo, wm);
	if (wm.y < 0.0f) {
		dotVH = -dotVH;
	}

	float ni = wo.y > 0.0f ? 1.0f : mat.ior;
	float nt = wo.y > 0.0f ? mat.ior : 1.0f;
	float relativeIOR = ni / nt;

	// -- Disney uses the full dielectric Fresnel equation for transmission. We also importance sample F
	// -- to switch between refraction and reflection at glancing angles.
	float F = FresnelDielectric(dotVH, 1.0f, mat.ior);

	// -- Since we're sampling the distribution of visible normals the pdf cancels out with a number of other terms.
	// -- We are left with the weight G2(wi, wo, wm) / G1(wi, wm) and since Disney uses a separable masking function
	// -- we get G1(wi, wm) * G1(wo, wm) / G1(wi, wm) = G1(wo, wm) as our weight.
	float G1v = SeparableSmithGGXG1(wo, wm, tax, tay);

	float pdf;

	vec3 wi;
	if (generate_random_float() <= F) {
		wi = reflect(wm, wo, 0.0).normalize();

		sample.reflectance = G1v * mat.clr;

		float jacobian = (4 * abs(dot(wo, wm)));
		pdf = F / jacobian;
	}
	else {
		if (thin) {
			// -- When the surface is thin so it refracts into and then out of the surface during this shading event.
			// -- So the ray is just reflected then flipped and we use the sqrt of the surface color.
			wi = reflect(wm, wo, 0.0f);
			wi.y = -wi.y;
			sample.reflectance = G1v * sqrt(mat.clr);

			// -- Since this is a thin surface we are not ending up inside of a volume so we treat this as a scatter event.
			sample.flags = SurfaceEventFlags::eScatterEvent;
		}
		else {
			if (Transmit(wm, wo, relativeIOR, wi)) {
				sample.flags = SurfaceEventFlags::eTransmissionEvent;
				sample.medium.phaseFunction = dotVH > 0.0f ? MediumPhaseFunction::eIsotropic : MediumPhaseFunction::eVacuum;
				sample.medium.extinction = CalculateExtinction(surface.transmittanceColor, surface.scatterDistance);
			}
			else {
				sample.flags = SurfaceEventFlags::eScatterEvent;
				wi = Reflect(wm, wo);
			}

			sample.reflectance = G1v * surface.baseColor;
		}

		wi = Normalize(wi);

		float dotLH = Absf(Dot(wi, wm));
		float jacobian = dotLH / (Square(dotLH + surface.relativeIOR * dotVH));
		pdf = (1.0f - F) / jacobian;
	}

	if (CosTheta(wi) == 0.0f) {
		sample.forwardPdfW = 0.0f;
		sample.reversePdfW = 0.0f;
		sample.reflectance = float3::Zero_;
		sample.wi = float3::Zero_;
		return false;
	}

	if (surface.roughness < 0.01f) {
		// -- This is a hack to allow us to sample the correct IBL texture when a path bounced off a smooth surface.
		sample.flags |= SurfaceEventFlags::eDiracEvent;
	}

	// -- calculate VNDF pdf terms and apply Jacobian and Fresnel sampling adjustments
	Bsdf::GgxVndfAnisotropicPdf(wi, wm, wo, tax, tay, sample.forwardPdfW, sample.reversePdfW);
	sample.forwardPdfW *= pdf;
	sample.reversePdfW *= pdf;

	// -- convert wi back to world space
	sample.wi = Normalize(MatrixMultiply(wi, MatrixTranspose(surface.worldToTangent)));

	return true;
}

*/


#endif
