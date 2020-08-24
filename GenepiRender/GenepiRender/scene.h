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


std::vector<triangle> scene_intersect(ray& r, float& t_near, std::vector<node>& trees)
{
    std::vector<triangle> hit_triangles;

    t_near = kinfinity;
    float t = kinfinity;

    for (auto& tree : trees)
    {
        if (tree.bbox_intersect(r, t))
        {
            for (auto child : tree.nodes)
            {
                if (child->bbox_intersect(r, t))
                {
                    hit_triangles.insert(hit_triangles.begin(),
                        child->tris.begin(),
                        child->tris.end());
                }
            }
            t_near = t;
        }
    }
    return hit_triangles;
}