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
#include "utils.h"


float infinity = std::numeric_limits<float>::infinity();


void load_scene(std::vector<material>& materials, RTCScene g_scene, RTCDevice g_device, std::vector<std::vector<vec3>>& geo_normals, const char* path)
{
    std::cout << "Loading scene...." << std::endl;
    auto start = std::chrono::system_clock::now();

    objl::Loader loader;

    bool loadout = loader.LoadFile(path);
    std::vector<vec3> normals;
    int tri_id = 0;

    if (loadout)
    {
        int id = 0;
        for (auto object : loader.LoadedMeshes)
        {   
            RTCGeometry geo = rtcNewGeometry(g_device, RTC_GEOMETRY_TYPE_TRIANGLE);
            Vertex* vertices = (Vertex*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertex), object.Vertices.size());

            for (int i = 0; i < object.Vertices.size(); i++)
            {
                vertices[i].x = object.Vertices[i].Position.X;
                vertices[i].y = object.Vertices[i].Position.Y;
                vertices[i].z = object.Vertices[i].Position.Z;
            }

            Triangle* triangles = (Triangle*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangle), object.Indices.size() / 3);
            
            normals.clear();

            int tri = 0;

            //load mesh faces into a mesh vector
            for (int i = 0; i < object.Indices.size(); i += 3)
            {
                triangles[tri].v0 = object.Indices[i];
                triangles[tri].v1 = object.Indices[i + 1];
                triangles[tri].v2 = object.Indices[i + 2];

                vec3 n0(object.Vertices[i].Normal.X, object.Vertices[i].Normal.Y, object.Vertices[i].Normal.Z);
                vec3 n1(object.Vertices[i + 1].Normal.X, object.Vertices[i + 1].Normal.Y, object.Vertices[i + 1].Normal.Z);
                vec3 n2(object.Vertices[i + 2].Normal.X, object.Vertices[i + 2].Normal.Y, object.Vertices[i + 2].Normal.Z);

                normals.push_back(n0);
                normals.push_back(n1);
                normals.push_back(n2);

                tri++;
            }

            geo_normals.push_back(normals);

            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_real_distribution<float> dist(0.25f, 0.75f);
            //vec3 rand_color(dist(mt));
            vec3 rand_color(0.75f);

            vec3 mat_color(object.MeshMaterial.Kd.X, object.MeshMaterial.Kd.Y, object.MeshMaterial.Kd.Z);

            float roughness = 1.0;
            float refrac = 0.0;
            //if (id == 0) rand_color = vec3(0.1f, 0.1f, 0.8f);
            if (id == 0) refrac = 1.0;
            if (id == 2) rand_color = vec3(1.f, 0.f, 0.f);
            if (id == 1) rand_color = vec3(0.f, 1.f, 0.f);

            material new_material(id, rand_color, roughness, refrac);

            materials.push_back(new_material);

            rtcCommitGeometry(geo);
            unsigned int geoID = rtcAttachGeometry(g_scene, geo);
            rtcReleaseGeometry(geo);
            id++;
        }
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Scene loaded in " << elapsed.count() << " seconds" << std::endl;
}


