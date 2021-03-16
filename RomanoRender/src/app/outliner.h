#pragma once


#include <cstdio>
#include <iostream>
#include "imgui.h"
#include "scene/camera.h"
#include "scene/scene.h"
#include "shading/light.h"
#include "console.h"

#ifndef OUTLINER
#define OUTLINER
 

enum class Selection_Type
{
	SelectionType_Object = 0x1,
	SelectionType_Camera = 0x2,
	SelectionType_Light  = 0x4
};


struct Outliner
{
	int selected = 0;
	Selection_Type selection_type;

	Outliner() {}

	~Outliner() {}

	void draw(std::vector<Object>& objects, std::vector<Camera>& cameras, std::vector<Light*>& lights, Console& console);
};


#endif