#pragma once


#include "utils/maths_utils.h"


float oren_nayar(const float sigma, const float R, const vec3& wo, const vec3& wi);

float ggx_normal_distribution(const float& NdotH, const float& roughness);

float schlick_masking_term(const float& NdotL, const float& NdotV, const float& roughness);

vec3 schlick_fresnel(const vec3& f0, const float& LdotH);

vec3 ggx_microfacet(const vec3& hit_normal, const float& roughness, const vec2& sample);