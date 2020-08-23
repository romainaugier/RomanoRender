#pragma once
#include <vector>


class tile
{
public:
	tile(int x_start, int x_end, int y_start, int y_end, int channels_num) :
		xstart(x_start),
		xend(x_end),
		ystart(y_start),
		yend(y_end),
		channels(channels_num)
	{
		size = (x_end - x_start) * (y_end - y_start) * channels;
	}


	int xstart, xend, ystart, yend;
	int number;
	int channels;
	int size;
	std::vector<float> data;
};

void set_tile_pixel(tile& cur_tile, float& r, float& g, float& b)
{
	cur_tile.data.push_back(r);
	cur_tile.data.push_back(g);
	cur_tile.data.push_back(b);
}


void set_tile_pixels(tile& cur_tile, float& r, float& g, float& b)
{
	int i = 0;
	while (i < cur_tile.size / 3)
	{
		cur_tile.data.push_back(r);
		cur_tile.data.push_back(g);
		cur_tile.data.push_back(b);
		i++;
	}
}

