#include "log.h"


void Logger::print(const char* message) const
{
	std::cout << level_index << " " << message << std::endl;
}

void Logger::print(std::string message) const
{
	std::cout << level_index << " " << message << std::endl;
}

void Logger::set_level(int lvl)
{
	level = lvl;
}