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


#ifndef RENDER
#define RENDER


// pathtracing integrator. Return a vec3 color
vec3 pathtrace(int s, std::vector<vec2>& sampler, const Ray& r, vec3 color, std::vector<Material>& mats, Render_Settings& settings, std::vector<Light*>& lights, int depth[], std::vector<int>& light_path, int samples[], Stats& stat);


// ao integrator
vec3 ambient_occlusion(int s, std::vector<vec2>& sampler, const Ray& r, Render_Settings& settings);


// scene viewer integrator
vec3 scene_viewer(const Ray& r, Render_Settings& settings);

// funtion used to render a single pixel, used for progressive rendering
void render_p(int s, std::vector<vec2>& sampler, color_t* pixels, int x, int y, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat);


// funtion used to render a single pixel, used for progressive rendering
void render_p_fast(int s, std::vector<vec2>& sampler, color_t* pixels, int x, int y, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat);


// function used to render progressively to the screen
void progressive_render(int s, int* ids, int y, std::vector<std::vector<vec2>>& sampler, color_t*& pixels, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat);


void progressive_render_fast(int s, int* ids, std::vector<std::vector<vec2>>& sampler, color_t*& pixels, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat);


#endif 