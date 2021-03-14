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
 


struct Outliner
{
	int selected = 0;

	Outliner() {}

	~Outliner() {}

	void draw(std::vector<Camera>& cameras, std::vector<Light*>& lights, Console& console);
};


#endif