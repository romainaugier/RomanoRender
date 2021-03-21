#pragma once

#include "scene/settings.h"
#include "scene/camera.h"
#include "utils/utils.h"

#ifndef RENDERSETTINGS
#define RENDERSETTINGS


struct Render_Settings_Window
{
	Render_Settings_Window() {}

	void draw(Render_Settings& settings, Render_View_Utils& utils, std::vector<Camera>& cameras, std::vector<std::vector<vec2>>& sequence, color_t* pixels, color_t* new_pixels, int* pixel_ids, int& sample_count, int& y, bool& change);
};


#endif // !RENDERSETTINGS

