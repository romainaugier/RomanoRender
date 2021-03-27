#pragma once


#include "imgui.h"

// needed to do operations with ImVec2
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <GL/gl3w.h> 
#include "OpenImageIO/imagebuf.h"
#include "utils/utils.h"

#ifndef RENDERVIEW
#define RENDERVIEW


struct Render_View
{
	Render_View() {}

	void draw(bool& render, Render_View_Utils& utils, int& s, int& y);
};


struct Render_View_Buttons
{
	Render_View_Buttons() {}
	void draw(bool& render, Render_View_Utils& utils, int& s, int& y, bool& save_window);
};


struct Save_Window
{
	Save_Window() {}

	void draw(int xres, int yres, int sample_count, color_t* image_buffer, bool& open);
};


#endif // !RENDERVIEW
