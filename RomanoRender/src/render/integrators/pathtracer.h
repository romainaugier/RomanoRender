#pragma once


#include "utils/maths_utils.h"
#include "utils/utils.h"
#include "utils/sampling_utils.h"
#include "utils/embree_utils.h"
#include "shading/material.h"
#include "shading/light.h"
#include "shading/bsdf.h"
#include "scene/settings.h"
#include "scene/camera.h"
#include "scene/stats.h"


#undef min
#undef max


vec3 pathtrace(int s, std::vector<vec2>& sampler, const Ray& r, vec3 color, std::vector<Material>& mats, Render_Settings& settings, std::vector<Light*>& lights, int depth[], std::vector<int>& light_path, int samples[], Stats& stat);