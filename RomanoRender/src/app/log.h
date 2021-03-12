#pragma once

#include <string>
#include <iostream>

#ifndef LOG
#define LOG

/*
log levels : 
0 = error
1 = warning
2 = verbose
3 = diagnostics
*/

struct Logger
{
public:
	int level;
	bool log_samples;
	bool log_scene;
	bool log_lights;
	bool log_stats;
	bool log_performances;
	
	const char* level_index;
	const char* message;

	Logger() {}

	Logger(int _level) :
		level(_level)
	{
		if (level == 0)
		{
			level_index = "[ERROR] :";
		}
		if (level == 1)
		{
			level_index = "[WARNING] :";
		}

		if (level == 2)
		{
			level_index = "[VERBOSE] :";
		}
		if (level == 3)
		{
			log_samples = true;
			log_scene = true;
			log_lights = true;
			log_stats = true;
			log_performances = true;
			level_index = "[DIAGNOSTIC] :";
		}
	}

	void print(const char* message) const;

	void print(std::string message) const;

	void set_level(int lvl);
};

#endif