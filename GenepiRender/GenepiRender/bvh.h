#pragma once
#include <vector>
#include <random>
#include <queue>
#include "ray.h"
#include "vec3.h"


class node
{
public:
	node() {}
	node(vec3 min_bound, vec3 max_bound, vec3 col, int tree_id) :
		min(min_bound),
		max(max_bound),
		color(col),
		id(tree_id),
		nodes(),
		tris() 
		{
			bounds[0] = min;
			bounds[1] = max;
		}

	bool bbox_intersect(const ray& r, float& t_min, float& t_max);
	std::vector<triangle> tree_intersect(const ray& r);

public:
	vec3 min, max;
	vec3 color;
	std::vector<triangle> tris;
	std::vector<node*> nodes;
	vec3 bounds[2];
	int id;
};


//void divide(node* tree, int count)
//{
//	//bbox division into n box depending on the count parameter
//  //init random color
//	std::random_device rd;
//	std::mt19937 mt(rd());
//	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
//
//	vec3 min = tree->min;
//	vec3 max = tree->max;
//
//	float maxx = fabs(max.x) + fabs(min.x);
//	float maxy = fabs(max.y) + fabs(min.y);
//	float maxz = fabs(max.z) + fabs(min.z);
//
//	vec3 min_axis(0.f);
//	vec3 max_axis(0.f);
//
//	for (int i = 0; i < count; i++)
//	{
//		if (maxx > maxy && maxx > maxz)
//		{
//			min_axis = vec3(min.x + ((maxx / count) * i), min.y, min.z);
//			max_axis = vec3(min.x + (maxx / count * (i + 1)), max.y - min.y, max.z - min.z);
//		}
//
//		if (maxy > maxx && maxy > maxz)
//		{
//			min_axis = vec3(min.x, min.y + ((maxy / count) * i), min.z);
//			max_axis = vec3(max.x - min.x, min.y + (maxy / count * (i + 1)), max.z - min.z);
//		}
//
//		if (maxz > maxx && maxz > maxy)
//		{
//			min_axis = vec3(min.x, min.y, min.z + ((maxz / count) * i));
//			max_axis = vec3(max.x - min.x, max.y - min.y, min.z + (maxz / count * (i + 1)));
//		}
//
//		vec3 rand_color(dist(mt));
//		tree->nodes.push_back(new node(min_axis, max_axis, rand_color));
//	}
//}


void divide(node* tree, int count, int special)
{
	//bbox division into n box depending on the count parameter
  //init random color
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	vec3 min = tree->min;
	vec3 max = tree->max;

	if (special == 0)
	{
		float maxx = fabs(max.x) + fabs(min.x);
		float maxy = fabs(max.y) + fabs(min.y);
		float maxz = fabs(max.z) + fabs(min.z);

		vec3 min_axis(0.f);
		vec3 max_axis(0.f);

		for (int y = 0; y < count; y++)
		{
			//min.y = min.y + ((maxy / count) * y);
			//max.y = min.y + (maxy / count * (y + 1));

			for (int z = 0; z < count; z++)
			{
				//min.z = min.z + ((maxz / count) * z);
				//max.z = min.z + (maxz / count * (z + 1));

				for (int x = 0; x < count; x++)
				{
					min_axis = vec3(min.x + ((maxx / count) * x), min.y + ((maxy / count) * y), min.z + ((maxz / count) * z));
					max_axis = vec3(min.x + (maxx / count * (x + 1)), min.y + (maxy / count * (y + 1)), min.z + (maxz / count * (z + 1)));
					vec3 rand_color(dist(mt));
					tree->nodes.push_back(new node(min_axis, max_axis, rand_color, 0));
				}
			}
		}
	}
	if(special > 0) tree->nodes.push_back(new node(min, max, vec3(1.0f), 0));
}


void push_triangles(node* tree, std::vector<triangle> triangles)
{
	for (int i = 0; i < tree->nodes.size(); i++)
	{
		vec3 min = tree->nodes[i]->min;
		vec3 max = tree->nodes[i]->max;

		for (std::vector<triangle>::iterator it = triangles.begin();
											 it != triangles.end(); it++)
		{
			vec3 vtx0 = sum(it->vtx0, it->vtx1, it->vtx2);
			vec3 vtx1 = it->vtx0;
			vec3 vtx2 = it->vtx1;
			vec3 vtx3 = it->vtx2;

			int count = 0;

			if (vtx0.x >= min.x && vtx0.x <= max.x &&
				vtx0.y >= min.y && vtx0.y <= max.y &&
				vtx0.z >= min.z && vtx0.z <= max.z)
				{
					count++;
				}

			if (vtx1.x >= min.x && vtx1.x <= max.x &&
				vtx1.y >= min.y && vtx1.y <= max.y &&
				vtx1.z >= min.z && vtx1.z <= max.z)
				{
					count++;
				}

			if (vtx2.x >= min.x && vtx2.x <= max.x &&
				vtx2.y >= min.y && vtx2.y <= max.y &&
				vtx2.z >= min.z && vtx2.z <= max.z)
				{
					count++;
				}

			if (vtx3.x >= min.x && vtx3.x <= max.x &&
				vtx3.y >= min.y && vtx3.y <= max.y &&
				vtx3.z >= min.z && vtx3.z <= max.z)
				{
					count++;
				}

			if (count > 0)
			{
				tree->nodes[i]->tris.emplace_back(*it);
			}
				//if (count == 3) triangles.erase(triangles.begin(), it);
			//else it++;
		}
	}
}


bool node::bbox_intersect(const ray& r, float& t_min, float& t_max)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (bounds[r.sign[0]].x - r.origin().x) * r.invdir.x;
	tmax = (bounds[1 - r.sign[0]].x - r.origin().x) * r.invdir.x;
	tymin = (bounds[r.sign[1]].y - r.origin().y) * r.invdir.y;
	tymax = (bounds[1 - r.sign[1]].y - r.origin().y) * r.invdir.y;

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	tzmin = (bounds[r.sign[2]].z - r.origin().z) * r.invdir.z;
	tzmax = (bounds[1 - r.sign[2]].z - r.origin().z) * r.invdir.z;

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	if (tmin < 0)
	{
		if (tmax < 0) return false;
	}

	t_min = tmin;
	t_max = tmax;

	return true;
}
