#pragma once

#ifndef STATS
#define STATS


class Stats
{
public:
	long long int rays;
	int samples;
	int poly_count;

public:
	Stats() {}

	Stats(int init) 
	{
		rays = 0;
		samples = 0;
		poly_count = 0;
	}

	void reset();

	void add_ray(); 
	void add_sample();
	void add_poly();
};









































#endif // !STATS
