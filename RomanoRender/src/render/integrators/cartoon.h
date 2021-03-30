#pragma once

#include "pathtracer.h"


vec3 cartoon(int s, std::vector<vec2>& sampler, int samples[], const Ray& r, std::vector<Material>& mats, std::vector<Light*>& lights, int depth[], Render_Settings& settings);