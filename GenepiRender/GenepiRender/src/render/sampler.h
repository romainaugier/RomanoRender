#pragma once

#include <iostream>
#include <fstream>
#include <regex>
#include "boost/filesystem.hpp"

#include "utils/vec2.h"


#ifndef SAMPLER
#define SAMPLER

std::vector<vec2> load_sample_sequence(const char* file)
{
	std::string line;
	std::fstream seq(file);

	if (seq.is_open())
	{
		std::vector<vec2> sequence;

        while (getline(seq, line))
        {
			double a, b;
			sscanf(line.c_str(), "(%lf, %lf),", &a, &b);

			vec2 sample(a, b);
			sequence.push_back(sample);
        }

        seq.close();

        return sequence;
	}
}


std::vector<std::vector<vec2>> load_sequences(const char* directory)
{
	std::vector<std::vector<vec2>> sequences;

	for (boost::filesystem::directory_entry& entry : boost::filesystem::directory_iterator(directory))
	{
		std::string to_s = entry.path().string();
		std::vector<vec2> seq = load_sample_sequence(to_s.c_str());
		sequences.push_back(seq);
	}

	return sequences;
}


#endif // !SAMPLER
