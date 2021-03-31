#include "ocio_utils.h"


OCIO::ConstConfigRcPtr initialize_ocio_config()
{
	try
	{
		return OCIO::GetCurrentConfig();
	}
	catch(OCIO::Exception & exception)
	{
		std::cerr << "OCIO : " << exception.what() << "\n";

		return OCIO::Config::CreateFromFile("C:/Program Files/OCIO/aces_1.2/config_chrisb.ocio");
	}
}


std::vector<char*> get_ocio_active_views(OCIO::ConstConfigRcPtr& config)
{
	const char* views = config->getActiveViews();
	const char* display = config->getActiveDisplays();
	const int views_count = config->getNumViews(display);
	const char s[2] = ",";

	std::vector<char*> views_arr;

	char* token = strtok((char*)views, s);

	while (token != NULL)
	{
		remove_spaces(token);
		views_arr.push_back(token);
		token = strtok(NULL, s);
	}

	return views_arr;
}


void ocio_color_pick_to_scene(vec3& color, OCIO::ConstConfigRcPtr& config)
{
	try
	{
		OCIO::ConstProcessorRcPtr processor = config->getProcessor(OCIO::ROLE_COLOR_PICKING, OCIO::ROLE_SCENE_LINEAR);
		OCIO::PackedImageDesc clr(&color.x, 1, 1, 3);
		processor->apply(clr);
	}
	catch (OCIO::Exception& exception)
	{
		std::cerr << "OCIO : " << exception.what() << "\n";
	}
}