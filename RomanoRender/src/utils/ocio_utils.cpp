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

		return OCIO::Config::CreateFromFile("C:/Program Files/OCIO/aces_1.0.3/config.ocio");
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