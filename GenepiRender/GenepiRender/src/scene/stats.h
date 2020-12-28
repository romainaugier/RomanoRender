#pragma once

#ifndef STATS
#define STATS


class stats
{
public:
	long long int rays;
	int samples;
	int poly_count;

public:
	stats() {}

	stats(int init) 
	{
		rays = 0;
		samples = 0;
		poly_count = 0;
	}

	void reset();

	void add_ray() { rays += 1; }
	void add_sample() { samples += 1; }
	void add_poly() { poly_count += 1;  }
};


void stats::reset()
{
	rays = 0;
	samples = 0;
}







































#endif // !STATS
