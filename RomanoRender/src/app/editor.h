#pragma once

#include "outliner.h"
#include "utils/matrix.h"
#include "utils/ocio_utils.h"


struct Editor
{
	Editor() {}

	void draw(Outliner& outliner, std::vector<Object>& objects, std::vector<Light*>& lights, std::vector<Camera>& cameras, OCIO::ConstConfigRcPtr& config, bool& edited);
};