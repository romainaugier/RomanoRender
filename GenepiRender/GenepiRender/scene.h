#pragma once
#include <vector>
#include "triangle.h"

class mesh
{
public:
	mesh() {}
	mesh(std::vector<triangle> mesh, vec3& _min, vec3& _max, int mesh_id) :
	id(mesh_id),
	min(_min),
	max(_max),
	tris(mesh)
	{}

public:
	int id;
	vec3 min, max;
	std::vector<triangle> tris;
};