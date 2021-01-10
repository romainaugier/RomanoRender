#pragma once
#include "scene/scene.h"
#include "scene/camera.h"


enum class Node_Type
{
	Geometry,
	Attributes,
	Surface,
	Output,
	Light,
	Camera,
	Settings,
	Merge,
	None
};


struct Node
{
	Node_Type type;
	int id;
	std::vector<Node> from;
	std::vector<Node> to;
	const char* name;
	std::vector<RTCGeometry> geometry;
	camera cam;
	char path[512] = "Path to your object";
	std::vector<material> materials;
	Light light;


	explicit Node(const Node_Type t) : type(t) 
	{
	}

	Node(const Node_Type t, int i, const char* n) : type(t), id(i), name(n) 
	{
	}
};


struct Link
{
	int id;
	int start_attr, end_attr;
};
