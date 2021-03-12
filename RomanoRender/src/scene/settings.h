#pragma once
#include "app/log.h"
#include "utils/embree_utils.h"


#ifndef SETTINGS
#define SETTINGS

class Render_Settings
{
public:
	int xres;
	int yres;
	int samples[3];
	int bounces[3];
	const char* output_path;
	Logger printer;
	int tile_number;
	RTCDevice device;
	RTCScene scene;

public:
	Render_Settings() {}

	Render_Settings(int x_res, int y_res, int _samples[], int _bounces[], Logger& _log, int tilenum) :
		xres(x_res),
		yres(y_res),
		samples{ _samples[0], _samples[1], _samples[2] },
		bounces{ _bounces[0], _bounces[1], _bounces[2] },
		//output_path(path),
		tile_number(tilenum),
		printer(_log)
	{}
};

#endif