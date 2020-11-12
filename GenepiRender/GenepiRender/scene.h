#pragma once
#include <vector>
#include <chrono>
#include <iostream>
#include <random>

#include <embree3/rtcore.h>

#include "scene.h"
#include "triangle.h"
#include "objloader.h"
#include "material.h"
#include "ray.h"

class mesh
{
public:
	mesh() {}
	mesh(std::vector<triangle> mesh, vec3& _min, vec3& _max, int& mesh_id) :
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

float infinity = std::numeric_limits<float>::infinity();


void errorFunction(void* userPtr, enum RTCError error, const char* str)
{
    printf("error %d: %s\n", error, str);
}


RTCDevice initializeDevice()
{
    RTCDevice device = rtcNewDevice(NULL);

    if (!device)
        printf("error %d: cannot create device\n", rtcGetDeviceError(NULL));

    rtcSetDeviceErrorFunction(device, errorFunction, NULL);
    return device;
}


void load_scene(std::vector<mesh>& scene, std::vector<material>& materials, RTCScene g_scene, RTCDevice g_device, const char* path)
{
    std::cout << "Loading scene...." << std::endl;
    auto start = std::chrono::system_clock::now();

    objl::Loader loader;

    bool loadout = loader.LoadFile(path);
    std::cout << loader.LoadedMeshes.size() << "\n";

    std::vector<triangle> faces;
    vec3 min(0.0f), max(0.0f);  

    int tri_id = 0;

    if (loadout)
    {
        int id = 0;
        for (auto object : loader.LoadedMeshes)
        {   
            RTCGeometry geo = rtcNewGeometry(g_device, RTC_GEOMETRY_TYPE_TRIANGLE);
            vec3* vertices = (vec3*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(vec3), object.Vertices.size());
            auto triangles = (triangle*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(triangle), object.Indices.size() / 3);
            std::cout << object.Indices.size() / 3 << "\n";
            
            int tri = 0;

            for (int i = 0; i < object.Vertices.size(); i++)
            {
                vertices[i].x = object.Vertices[i].Position.X;
                vertices[i].y = object.Vertices[i].Position.Y;
                vertices[i].z = object.Vertices[i].Position.Z;
            }

            //load mesh faces into a mesh vector
            for (int i = 0; i < object.Indices.size(); i += 3)
            {
                int vtx_idx0 = object.Indices[i];
                int vtx_idx1 = object.Indices[i + 1];
                int vtx_idx2 = object.Indices[i + 2];

                triangles[tri].v0 = vec3(object.Vertices[vtx_idx0].Position.X, object.Vertices[vtx_idx0].Position.Y, object.Vertices[vtx_idx0].Position.Z);
                triangles[tri].v1 = vec3(object.Vertices[vtx_idx1].Position.X, object.Vertices[vtx_idx1].Position.Y, object.Vertices[vtx_idx1].Position.Z);
                triangles[tri].v2 = vec3(object.Vertices[vtx_idx2].Position.X, object.Vertices[vtx_idx2].Position.Y, object.Vertices[vtx_idx2].Position.Z);

                triangles[tri].n0 = vec3(object.Vertices[vtx_idx0].Normal.X, object.Vertices[vtx_idx0].Normal.Y, object.Vertices[vtx_idx0].Normal.Z);
                triangles[tri].n1 = vec3(object.Vertices[vtx_idx1].Normal.X, object.Vertices[vtx_idx1].Normal.Y, object.Vertices[vtx_idx1].Normal.Z);
                triangles[tri].n2 = vec3(object.Vertices[vtx_idx2].Normal.X, object.Vertices[vtx_idx2].Normal.Y, object.Vertices[vtx_idx2].Normal.Z);

                triangles[tri].t0 = vec3(object.Vertices[vtx_idx0].TextureCoordinate.X, object.Vertices[vtx_idx0].TextureCoordinate.Y, 0.0f);
                triangles[tri].t1 = vec3(object.Vertices[vtx_idx1].TextureCoordinate.X, object.Vertices[vtx_idx1].TextureCoordinate.Y, 0.0f);
                triangles[tri].t2 = vec3(object.Vertices[vtx_idx2].TextureCoordinate.X, object.Vertices[vtx_idx2].TextureCoordinate.Y, 0.0f);

                triangles[tri].tri_id = tri_id;
                triangles[tri].mat_id = id;

                tri_id++;
                tri++;
            }

            id++;

            rtcCommitGeometry(geo);
            unsigned int geoID = rtcAttachGeometry(g_scene, geo);
            rtcReleaseGeometry(geo);
        }
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Scene loaded in " << elapsed.count() << " seconds" << std::endl;
}

/*
std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<float> dist(0.25f, 0.75f);
//vec3 rand_color(dist(mt));
vec3 rand_color(1.f);

//vec3 mat_color(object.MeshMaterial.Kd.X, object.MeshMaterial.Kd.Y, object.MeshMaterial.Kd.Z);

float roughness = 1.0;
float refrac = 0.0;
//if (id == 0) roughness = 0.5;
if (id == 0) refrac = 1.0;
if (id == 2) rand_color = vec3(1.f, 0.f, 0.f);
if (id == 1) rand_color = vec3(0.f, 1.f, 0.f);

material new_material(id, rand_color, roughness, refrac);
mesh new_mesh(faces, min, max, id);
scene.push_back(new_mesh);
materials.push_back(new_material);
*/