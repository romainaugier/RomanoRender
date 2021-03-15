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


struct Render_View_Utils
{
	GLuint render_view_texture;
	ImVec2 scrolling;
	ImVec2 resolution;
	float zoom = 1.0f;
	color_t* buffer1, *buffer2;

	Render_View_Utils(float xres, float yres, color_t* buffer1, color_t* buffer2) 
	{
		// initializing texture for the renderview
		glGenTextures(1, &render_view_texture);
		glBindTexture(GL_TEXTURE_2D, render_view_texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xres, yres, 0, GL_RGB, GL_FLOAT, buffer1);

		glBindTexture(GL_TEXTURE_2D, 0);

		scrolling = ImVec2(0.0f, 0.0f);

		resolution = ImVec2(xres, yres);
	}
};


struct Render_View
{
	Render_View() {}

	void draw(bool& render, Render_View_Utils& utils, int& s, int& y);
};


struct Render_View_Buttons
{
	Render_View_Buttons() {}
	void draw(bool& render, Render_View_Utils& utils, int& s, int& y);
};


struct Save_Window
{
	Save_Window() {}

	void draw(int xres, int yres, int sample_count, color_t* image_buffer, bool& open);
};


#endif // !RENDERVIEW
