#include "stats.h"


void Stats::reset()
{
	rays = 0;
	samples = 0;
}

void Stats::add_ray() { rays += 1; }

void Stats::add_sample() { samples += 1; }

void Stats::add_poly() { poly_count += 1;  }