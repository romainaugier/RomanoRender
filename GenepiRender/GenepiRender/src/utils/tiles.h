#pragma once
#include <vector>

#ifndef TILE
#define TILE


class tile
{
public:
	tile(int x_start, int x_end, int y_start, int y_end, int channels_num, int tile_id) :
		xstart(x_start),
		xend(x_end),
		ystart(y_start),
		yend(y_end),
		channels(channels_num),
		id(tile_id)
	{
		size = (x_end - x_start) * (y_end - y_start) * channels;
	}


	int xstart, xend, ystart, yend;
	int number;
	int channels;
	int size;
	int id;
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


std::vector<tile> generate_tiles(const int& number, const int& xres, const int& yres, const int& channels)
{
	std::vector<tile> tiles;

	int id = 0;

	for (int y = 0; y < number; y++)
	{
		int miny = yres / number * y;
		int maxy = yres / number * (y + 1);

		for (int x = 0; x < number; x++)
		{
			int minx = xres / number * x;
			int maxx = xres / number * (x + 1);

			tile new_tile(minx, maxx, miny, maxy, channels, id);
			tiles.push_back(new_tile);
			id++;
		}
	}
	return tiles;
}


#endif