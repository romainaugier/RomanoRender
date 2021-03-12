#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <regex>

#include "boost/filesystem.hpp"

#include "vec3.h"
#include "vec2.h"
#include "maths_utils.h"

// to use sscanf without errors
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 
#endif


// sampling utils
vec3 sample_ray_in_hemisphere(const vec3& hit_normal, const vec2& sample);
vec3 sample_ray_in_sphere();
vec3 sample_unit_disk();

// random generator utils
int wang_hash(int seed);
int xorshift32(int state);
float generate_random_float_slow();
float generate_random_float_fast(int state);
float exponential_distribution(float& sigma);

// sample sequence loading utils
std::vector<vec2> load_sample_sequence(const char* file);
std::vector<std::vector<vec2>> load_sequences(const char* directory);
