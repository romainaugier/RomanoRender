#pragma once

#include "pathtracer.h"

vec3 ambient_occlusion(int s, std::vector<vec2>& sampler, const Ray& r, Render_Settings& settings);