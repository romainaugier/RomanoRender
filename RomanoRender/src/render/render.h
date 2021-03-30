#pragma once

#include "integrators/pathtracer.h"
#include "integrators/ao.h"
#include "integrators/scene_view.h"
#include "integrators/cartoon.h"


#ifndef RENDER
#define RENDER


// funtion used to render a single pixel, used for progressive rendering
void render_p(int s, std::vector<vec2>& sampler, color_t* pixels, int x, int y, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat);


// funtion used to render a single pixel, used for progressive rendering
void render_p_fast(int s, std::vector<vec2>& sampler, color_t* pixels, int x, int y, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat);


// function used to render progressively to the screen
void progressive_render(int s, int* ids, int y, std::vector<std::vector<vec2>>& sampler, color_t*& pixels, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat);


void progressive_render_fast(int s, int* ids, std::vector<std::vector<vec2>>& sampler, color_t*& pixels, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat);


#endif 