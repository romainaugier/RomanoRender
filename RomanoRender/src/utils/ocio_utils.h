#pragma once


#ifndef OCIO_UTILS
#define OCIO_UTILs

#include "OpenColorIO/OpenColorIO.h"
#include <string.h>
#include <stdio.h>
#include <vector>

#include "str_utils.h"

namespace OCIO = OCIO_NAMESPACE;


// initialize the ocio configuration at the app start
OCIO::ConstConfigRcPtr initialize_ocio_config();

// get all the views as separated tokens
std::vector<char*> get_ocio_active_views(OCIO::ConstConfigRcPtr& config);


struct OCIO_Utils
{
	char* current_view;
	char* current_display;
	std::vector<char*> active_views;

	OCIO_Utils(OCIO::ConstConfigRcPtr& config)
	{
		current_display = (char*)config->getDefaultDisplay();
		current_view = (char*)config->getDefaultView(current_display);
		active_views = get_ocio_active_views(config);
	}
};





#endif // !OCIO
