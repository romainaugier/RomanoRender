#pragma once
#include <vector>
#include "ray.h"
#include "vec3.h"

/*void divide(node* node, int depth)
{
    // some real division logic
    // node->childs[0] = new node(<bounds>)
    // node->childs[1] = new node(<bounds>)
    // node->childs[2] = new node(<bounds>)
    // ...
    
    if (depth < N)
    {
        for (auto child : node->childs)
        {
            divide(child, ++depth);
        }
    }
}*/

class node
{
public:
	node(vec3 min_bound, vec3 max_bound, bool is_leaf) :
		min(min_bound),
		max(max_bound),
		leaf(is_leaf),
		children(8) {}

	bool bbox_intersect(const ray& r);
	bool tree_intersect(ray r);

public:
	vec3 min, max;
	bool leaf;
	std::vector<node*> children;
};


void divide(node* tree, int depth, bool is_leaf)
{
	//recursive bbox division

	//box1 : min(min), max(max/2)
	vec3 bx1_min(tree->min);
	vec3 bx1_max(tree->max / 2);
	tree->children[0] = new node(bx1_min, bx1_max, is_leaf);
	//std::cout << is_leaf << std::endl;
	//std::cout << bx1_min;
	//std::cout << bx1_max << std::endl;											//debug

	//box2 : min(max.x/2, min.y, min.z), max(max.x, max.y/2, max.z/2)
	vec3 bx2_min(tree->min.x / 2, tree->min.y, tree->min.z / 2);
	vec3 bx2_max(tree->max.x, tree->min.y / 2, tree->min.z);
	tree->children[1] = new node(bx2_min, bx2_max, is_leaf);

	//box3 : min(max.x/2, min.y, max.z/2), max(max.x, max.y/2, max.z)
	vec3 bx3_min(tree->max.x / 2, tree->min.y, tree->max.z / 2);
	vec3 bx3_max(tree->max.x, tree->max.y / 2, tree->max.z);
	tree->children[2] = new node(bx3_min, bx3_max, is_leaf);

	//box4 : min(min.x, min.y, max.z/2), max(max.x/2, max.y/2, max.z)
	vec3 bx4_min(tree->min.x, tree->max.y / 2, tree->min.z);
	vec3 bx4_max(tree->max.x / 2, tree->max.y / 2, tree->max.z);
	tree->children[3] = new node(bx4_min, bx4_max, is_leaf);

	//box5 : min(min.x, max.y/2, min.z), max(max.x/2, max.y, max.z/2)
	vec3 bx5_min(tree->min.x, tree->max.y / 2, tree->min.z);
	vec3 bx5_max(tree->max.x / 2, tree->max.y, tree->max.z / 2);
	tree->children[4] = new node(bx5_min, bx5_max, is_leaf);

	//box6 : min(max.x/2, max.y/2, min.z), max(max.x, max.y, max.z/2)
	vec3 bx6_min(tree->max.x / 2, tree->max.y / 2, tree->min.z);
	vec3 bx6_max(tree->max.x, tree->max.y, tree->max.z / 2);
	tree->children[5] = new node(bx6_min, bx6_max, is_leaf);

	//box7 : min(max.x/2, max.y/2, max.z/2), max(max.x, max.y, max.z)
	vec3 bx7_min(tree->max.x / 2, tree->max.y / 2, tree->max.z / 2);
	vec3 bx7_max(tree->max);
	tree->children[6] = new node(bx7_min, bx7_max, is_leaf);

	//box8 : min(min.x, max.y/2, max.z/2), max(max.x/2, max.y, max.z)
	vec3 bx8_min(tree->min.x, tree->max.y / 2, tree->max.z / 2);
	vec3 bx8_max(tree->max.x / 2, tree->max.y, tree->max.z);
	tree->children[7] = new node(bx8_min, bx8_max, is_leaf);

	int N = 3;
	depth++;

	if (depth < N)
	{
		
		if (depth == (N - 1)) is_leaf = true;
		
		for (auto child : tree->children)
		{
			divide(child, depth, is_leaf);
		}
	}
	
}


void push_tris(std::vector<triangle>& mesh, node* leaf)
{
	//if triangle in leaf->bounds
	//  push triangle
	//  remove triangle from mesh vector
	//if one vertex in leaf-bounds
	//  push triangle
}


//void build_tree(std::vector<mesh>& meshes, int depth)
//{
	/*
	Calculate world box
	For each mesh:
	  node = calculate bounding box
	  divide(node, depth)
	*/
//}


bool node::bbox_intersect(const ray& r)
{
	float tmin, tmax;

	float txmin = (min.x - r.origin().x) / r.direction().x;
	float txmax = (max.x - r.origin().x) / r.direction().x;

	if (txmin > txmax) std::swap(txmin, txmax);

	float tymin = (min.y - r.origin().y) / r.direction().y;
	float tymax = (max.y - r.origin().y) / r.direction().y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((txmin > tymax) || (tymin > txmax))
		return false;

	if (tymin > txmin)
		txmin = tymin;

	if (tymax < txmax)
		txmax = tymax;

	float tzmin = (min.z - r.origin().z) / r.direction().z;
	float tzmax = (max.z - r.origin().z) / r.direction().z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((txmin > tzmax) || (tzmin > txmax))
		return false;

	if (tzmin > txmin)
		txmin = tzmin;

	if (tzmax < txmax)
		txmax = tzmax;

	tmin = txmin;
	tmax = txmax;

	return true;
}


void tree_info(node* tree, int size)
{
	size += tree->children.size();

	for (auto child : tree->children)
	{
		size++;
		tree_info(child, size);
	}
}