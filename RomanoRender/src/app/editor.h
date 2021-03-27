#pragma once

#include "outliner.h"
#include "utils/matrix.h"


struct Editor
{
	Editor() {}

	void draw(Outliner& outliner, std::vector<Object>& objects, std::vector<Light*>& lights, std::vector<Camera>& cameras, bool& edited);
};