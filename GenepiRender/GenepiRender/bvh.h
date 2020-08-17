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

struct node { vec3 min; vec3 max; std::vector<node*> children; };

void divide(node* tree, int depth)
{
	//recursive bbox division 
	//box1 : min(min), max(max/2)

	vec3 bx1_min(tree->min);
	vec3 bx1_max(tree->max / 2);
	std::vector<node*> box1_children;
	tree->children[0] = new node(bx1_min, bx1_max, nullptr);
	


	//box2 : min(max.x/2, min.y, min.z), max(max.x, max.y/2, max.z/2)
	std::vector<node> box2;
	vec3 bx2_min(tree->min.x / 2, tree->min.y, tree->min.z / 2);
	vec3 bx2_max(tree->max.x, tree->min.y / 2, tree->min.z);
	node box2_bounds(bx2_min, bx2_max);
	box2.push_back(box2_bounds);
	tree.push_back(box2);
	temp.push_back(box2);

	//box3 : min(max.x/2, min.y, max.z/2), max(max.x, max.y/2, max.z)
	std::vector<node> box3;
	vec3 bx3_min(bounds[1].x / 2, bounds[0].y, bounds[1].z / 2);
	vec3 bx3_max(bounds[1].x, bounds[1].y / 2, bounds[1].z);
	node box3_bounds(bx3_min, bx3_max);
	box3.push_back(box3_bounds);
	tree.push_back(box3);
	temp.push_back(box3);

	//box4 : min(min.x, min.y, max.z/2), max(max.x/2, max.y/2, max.z)
	std::vector<node> box4;
	vec3 bx4_min(bounds[0].x, bounds[1].y / 2, bounds[0].z);
	vec3 bx4_max(bounds[1].x / 2, bounds[1].y / 2, bounds[1].z);
	node box4_bounds(bx4_min, bx4_max);
	box4.push_back(box4_bounds);
	tree.push_back(box4);
	temp.push_back(box4);

	//box5 : min(min.x, max.y/2, min.z), max(max.x/2, max.y, max.z/2)
	std::vector<node> box5;
	vec3 bx5_min(bounds[0].x, bounds[1].y / 2, bounds[0].z);
	vec3 bx5_max(bounds[1].x / 2, bounds[1].y, bounds[1].z / 2);
	node box5_bounds(bx5_min, bx5_max);
	box5.push_back(box5_bounds);
	tree.push_back(box5);
	temp.push_back(box5);

	//box6 : min(max.x/2, max.y/2, min.z), max(max.x, max.y, max.z/2)
	std::vector<node> box6;
	vec3 bx6_min(bounds[1].x / 2, bounds[1].y / 2, bounds[0].z);
	vec3 bx6_max(bounds[1].x, bounds[1].y, bounds[1].z / 2);
	node box6_bounds(bx6_min, bx6_max);
	box6.push_back(box6_bounds);
	tree.push_back(box6);
	temp.push_back(box6);

	//box7 : min(max.x/2, max.y/2, max.z/2), max(max.x, max.y, max.z)
	std::vector<node> box7;
	vec3 bx7_min(bounds[1].x / 2, bounds[1].y / 2, bounds[1].z / 2);
	vec3 bx7_max(bounds[1]);
	node box7_bounds(bx7_min, bx7_max);
	box7.push_back(box7_bounds);
	tree.push_back(box7);
	temp.push_back(box7);

	//box8 : min(min.x, max.y/2, max.z/2), max(max.x/2, max.y, max.z)
	std::vector<node> box8;
	vec3 bx8_min(bounds[0].x, bounds[1].y / 2, bounds[1].z / 2);
	vec3 bx8_max(bounds[1].x / 2, bounds[1].y, bounds[1].z);
	node box8_bounds(bx8_min, bx8_max);
	box8.push_back(box8_bounds);
	tree.push_back(box8);
	temp.push_back(box8);
}

/*
class node
{
public:
	node(vec3& min, vec3& max) { bounds[0] = min, bounds[1] = max; }

	bool bbox_intersect(const ray& r);
	std::vector<std::vector<node>> build_tree(std::vector<std::vector<node>> tree, vec3 min, vec3 max, int depth);
	node divide(std::vector<std::vector<node>> tree, std::vector<std::vector<node>> temp);
public:
	vec3 bounds[2];

};

node node::divide(std::vector<std::vector<node>> tree, std::vector<std::vector<node>> temp)
{
	//recursive bbox division 
	//box1 : min(min), max(max/2)
	std::vector<std::vector<node>> box1;
	vec3 bx1_min(bounds[0]);
	vec3 bx1_max(bounds[1] / 2);
	node node_box1_bounds(bx1_min, bx1_max);
	std::vector<node> box1_bounds;
	box1_bounds.push_back(node_box1_bounds);
	box1.push_back(box1_bounds);
	tree.push_back(box1);
	temp.push_back(box1);

	//box2 : min(max.x/2, min.y, min.z), max(max.x, max.y/2, max.z/2)
	std::vector<node> box2;
	vec3 bx2_min(bounds[1].x / 2, bounds[0].y, bounds[1].z / 2);
	vec3 bx2_max(bounds[1].x, bounds[1].y / 2, bounds[1].z);
	node box2_bounds(bx2_min, bx2_max);
	box2.push_back(box2_bounds);
	tree.push_back(box2);
	temp.push_back(box2);

	//box3 : min(max.x/2, min.y, max.z/2), max(max.x, max.y/2, max.z)
	std::vector<node> box3;
	vec3 bx3_min(bounds[1].x / 2, bounds[0].y, bounds[1].z / 2);
	vec3 bx3_max(bounds[1].x, bounds[1].y / 2, bounds[1].z);
	node box3_bounds(bx3_min, bx3_max);
	box3.push_back(box3_bounds);
	tree.push_back(box3);
	temp.push_back(box3);

	//box4 : min(min.x, min.y, max.z/2), max(max.x/2, max.y/2, max.z)
	std::vector<node> box4;
	vec3 bx4_min(bounds[0].x, bounds[1].y / 2, bounds[0].z);
	vec3 bx4_max(bounds[1].x / 2, bounds[1].y / 2, bounds[1].z);
	node box4_bounds(bx4_min, bx4_max);
	box4.push_back(box4_bounds);
	tree.push_back(box4);
	temp.push_back(box4);

	//box5 : min(min.x, max.y/2, min.z), max(max.x/2, max.y, max.z/2)
	std::vector<node> box5;
	vec3 bx5_min(bounds[0].x, bounds[1].y / 2, bounds[0].z);
	vec3 bx5_max(bounds[1].x / 2, bounds[1].y, bounds[1].z / 2);
	node box5_bounds(bx5_min, bx5_max);
	box5.push_back(box5_bounds);
	tree.push_back(box5);
	temp.push_back(box5);

	//box6 : min(max.x/2, max.y/2, min.z), max(max.x, max.y, max.z/2)
	std::vector<node> box6;
	vec3 bx6_min(bounds[1].x / 2, bounds[1].y / 2, bounds[0].z);
	vec3 bx6_max(bounds[1].x, bounds[1].y, bounds[1].z / 2);
	node box6_bounds(bx6_min, bx6_max);
	box6.push_back(box6_bounds);
	tree.push_back(box6);
	temp.push_back(box6);

	//box7 : min(max.x/2, max.y/2, max.z/2), max(max.x, max.y, max.z)
	std::vector<node> box7;
	vec3 bx7_min(bounds[1].x / 2, bounds[1].y / 2, bounds[1].z / 2);
	vec3 bx7_max(bounds[1]);
	node box7_bounds(bx7_min, bx7_max);
	box7.push_back(box7_bounds);
	tree.push_back(box7);
	temp.push_back(box7);

	//box8 : min(min.x, max.y/2, max.z/2), max(max.x/2, max.y, max.z)
	std::vector<node> box8;
	vec3 bx8_min(bounds[0].x, bounds[1].y / 2, bounds[1].z / 2);
	vec3 bx8_max(bounds[1].x / 2, bounds[1].y, bounds[1].z);
	node box8_bounds(bx8_min, bx8_max);
	box8.push_back(box8_bounds);
	tree.push_back(box8);
	temp.push_back(box8);
}


std::vector<std::vector<node>> node::build_tree(std::vector<std::vector<node>> tree, vec3 min, vec3 max, int depth)
{
	std::vector<std::vector<node>> temp;
	std::vector<node> root;
	node root_node(min, max);
	root.push_back(root_node);
	tree.push_back(root);
	for (int i = 0; i < depth; i++)
	{
		temp.push_back(root);
		for (int j = 0; j < temp.size(); j++)
		{

		}
	}
}

*/

bool node::bbox_intersect(const ray& r)
{
	float tmin, tmax;

	float txmin = (bounds[0].x - r.origin().x) / r.direction().x;
	float txmax = (bounds[1].x - r.origin().x) / r.direction().x;

	if (txmin > txmax) std::swap(txmin, txmax);

	float tymin = (bounds[0].y - r.origin().y) / r.direction().y;
	float tymax = (bounds[1].y - r.origin().y) / r.direction().y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((txmin > tymax) || (tymin > txmax))
		return false;

	if (tymin > txmin)
		txmin = tymin;

	if (tymax < txmax)
		txmax = tymax;

	float tzmin = (bounds[0].z - r.origin().z) / r.direction().z;
	float tzmax = (bounds[1].z - r.origin().z) / r.direction().z;

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