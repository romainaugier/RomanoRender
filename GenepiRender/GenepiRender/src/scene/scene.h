#pragma once
#include <vector>
#include <chrono>
#include <iostream>
#include <random>

#include <embree3/rtcore.h>

#include "scene.h"
#include "objloader.h"
#include "shading/material.h"
#include "utils/ray.h"
#include "utils/utils.h"
#include "shading/light.h"
#include "stats.h"


#ifndef SCENE
#define SCENE


void load_scene(std::vector<material>& materials, std::vector<light>& lights, RTCScene g_scene, RTCDevice g_device, const char* path, stats& stat)
{
    std::cout << "Loading scene...." << std::endl;
    auto start = std::chrono::system_clock::now();

    objl::Loader loader;

    bool loadout = loader.LoadFile(path);
    int tri_id = 0;
    int id = 0;

    // load geometry
    if (loadout)
    {
        
        for (auto object : loader.LoadedMeshes)
        {   
            /*
            RTCGeometry geo = rtcNewGeometry(g_device, RTC_GEOMETRY_TYPE_SUBDIVISION);


            Vertex* vertices = (Vertex*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertex), object.Vertices.size());

            for (int i = 0; i < object.Vertices.size(); i++)
            {
                vertices[i].x = object.Vertices[i].Position.X;
                vertices[i].y = object.Vertices[i].Position.Y;
                vertices[i].z = object.Vertices[i].Position.Z;
            }

            Vertex* normals = new Vertex[object.Vertices.size()];

            for (int i = 0; i < object.Vertices.size(); i++)
            {
                normals[i].x = object.Vertices[i].Normal.X;
                normals[i].y = object.Vertices[i].Normal.Y;
                normals[i].z = object.Vertices[i].Normal.Z;
            }


            rtcSetGeometryVertexAttributeCount(geo, 1);
            rtcSetSharedGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, normals, 0, sizeof(Vertex), object.Vertices.size());

            

            int* triangles = (int*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT, sizeof(int), object.Indices.size());

            for (int i = 0; i < object.Indices.size(); i++)
            {
                triangles[i] = object.Indices[i];
            }


            int* faces = (int*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_FACE, 0, RTC_FORMAT_UINT, sizeof(unsigned int), (unsigned int)object.Indices.size() / 3);
            
            for (int i = 0; i < object.Indices.size() / 3; i++)
            {
                faces[i] = 3;
            }

            float* level = (float*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_LEVEL, 0, RTC_FORMAT_FLOAT, sizeof(float), object.Indices.size());

            for (unsigned int i = 0; i < object.Indices.size(); i++)
                level[i] = 8;
            */

            RTCGeometry geo = rtcNewGeometry(g_device, RTC_GEOMETRY_TYPE_TRIANGLE);
            Vertex* vertices = (Vertex*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertex), object.Vertices.size());

            for (int i = 0; i < object.Vertices.size(); i++)
            {
                vertices[i].x = object.Vertices[i].Position.X;
                vertices[i].y = object.Vertices[i].Position.Y;
                vertices[i].z = object.Vertices[i].Position.Z;
            }

            Vertex* normals = new Vertex[object.Vertices.size()];

            for (int i = 0; i < object.Vertices.size(); i++)
            {
                normals[i].x = object.Vertices[i].Normal.X;
                normals[i].y = object.Vertices[i].Normal.Y;
                normals[i].z = object.Vertices[i].Normal.Z;
            }

            rtcSetGeometryVertexAttributeCount(geo, 1);
            rtcSetSharedGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, normals, 0, sizeof(Vertex), object.Vertices.size());

            Triangle* triangles = (Triangle*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangle), object.Indices.size() / 3);

            int tri = 0;

            for (int i = 0; i < object.Indices.size(); i += 3)
            {
                triangles[tri].v0 = object.Indices[i];
                triangles[tri].v1 = object.Indices[i + 1];
                triangles[tri].v2 = object.Indices[i + 2];

                stat.add_poly();

                tri++;
            }


            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_real_distribution<float> dist(0.25f, 0.75f);
            //vec3 rand_color(dist(mt));
            vec3 rand_color(0.18f);

            vec3 mat_color(object.MeshMaterial.Kd.X, object.MeshMaterial.Kd.Y, object.MeshMaterial.Kd.Z);

            float roughness = 1.0f;
            float refrac = 0.0f;

            //if (id == 0) mat_color = vec3(1.0f, 1.0f, 1.0f);
            //if (id == 0) refrac = 1.0f;
            //if (id == 0) roughness = 0.99;
            //if (id == 2) rand_color = vec3(0.f, 1.0f, 0.f);
            //if (id == 4) rand_color = vec3(1.0f, 0.f, 0.f);


            material new_material(id, object.MeshName, mat_color, roughness, refrac);

            new_material.islight = false;
            
            materials.push_back(new_material);

            rtcCommitGeometry(geo);
            unsigned int geoID = rtcAttachGeometry(g_scene, geo);
            rtcReleaseGeometry(geo);
            id++;
        }
    }
    
    
    for (auto light : lights)
    {
        if (light.type == 2)
        {
            if (light.visible == false) continue;

            RTCGeometry geo = rtcNewGeometry(g_device, RTC_GEOMETRY_TYPE_TRIANGLE);
            Vertex* vertices = (Vertex*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertex), 4);

            vertices[0].x = light.v0.x;
            vertices[0].y = light.v0.y;
            vertices[0].z = light.v0.z;

            vertices[1].x = light.v1.x;
            vertices[1].y = light.v1.y;
            vertices[1].z = light.v1.z;

            vertices[2].x = light.v2.x;
            vertices[2].y = light.v2.y;
            vertices[2].z = light.v2.z;

            vertices[3].x = light.v3.x;
            vertices[3].y = light.v3.y;
            vertices[3].z = light.v3.z;

            Vertex* normals = new Vertex[4];

            for (int i = 0; i < 4; i++)
            {
                normals[i].x = light.orientation.x;
                normals[i].y = light.orientation.y;
                normals[i].z = light.orientation.z;
            }

            rtcSetGeometryVertexAttributeCount(geo, 1);
            rtcSetSharedGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, normals, 0, sizeof(Vertex), 4);

            Triangle* triangles = (Triangle*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangle), 2);

            triangles[0].v0 = 0;
            triangles[0].v1 = 1;
            triangles[0].v2 = 2;

            triangles[1].v0 = 2;
            triangles[1].v1 = 3;
            triangles[1].v2 = 1;

            material new_material(id, light.color, light.orientation, true);
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


#endif