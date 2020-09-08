#include <vector>
#include <chrono>
#include <iostream>
#include <random>

#include "scene.h"
#include "triangle.h"
#include "objloader.h"
#include "material.h"


static float infinity = std::numeric_limits<float>::max();


void load_scene(std::vector<mesh>& scene, std::vector<material>& materials)
{
    std::cout << "Loading scene...." << std::endl;
    auto start = std::chrono::system_clock::now();

    objl::Loader loader;

    float min_x = 0.0, max_x = 0.0, min_y = 0.0, max_y = 0.0, min_z = 0.0, max_z = 0.0;

    bool loadout = loader.LoadFile("D:/GenepiRender/Models/scene_drag.obj");

    std::vector<triangle> faces;

    vec3 min(0.0f), max(0.0f);

    if (loadout)
    {
        int id = 0;
        for (auto object : loader.LoadedMeshes)
        {
            faces.clear();

            min_x = infinity;
            max_x = -infinity;
            min_y = infinity;
            max_y = -infinity;
            min_z = infinity;
            max_z = -infinity;


            for (int j = 0; j < object.Vertices.size(); j++)
            {
                float px = object.Vertices[j].Position.X;
                float py = object.Vertices[j].Position.Y;
                float pz = object.Vertices[j].Position.Z;

                if (px < min_x) min_x = px;
                if (px > max_x) max_x = px;
                if (py < min_y) min_y = py;
                if (py > max_y) max_y = py;
                if (pz < min_z) min_z = pz;
                if (pz > max_z) max_z = pz;
            }

            min.x = min_x;
            min.y = min_y;
            min.z = min_z;

            max.x = max_x;
            max.y = max_y;
            max.z = max_z;

            //load mesh faces into a mesh vector
            for (int i = 0; i < object.Indices.size(); i += 3)
            {
                int vtx_idx0 = object.Indices[i];
                int vtx_idx1 = object.Indices[i + 1];
                int vtx_idx2 = object.Indices[i + 2];

                vec3 vtx_0(object.Vertices[vtx_idx0].Position.X, object.Vertices[vtx_idx0].Position.Y, object.Vertices[vtx_idx0].Position.Z);
                vec3 vtx_1(object.Vertices[vtx_idx1].Position.X, object.Vertices[vtx_idx1].Position.Y, object.Vertices[vtx_idx1].Position.Z);
                vec3 vtx_2(object.Vertices[vtx_idx2].Position.X, object.Vertices[vtx_idx2].Position.Y, object.Vertices[vtx_idx2].Position.Z);

                vec3 n_0(object.Vertices[vtx_idx0].Normal.X, object.Vertices[vtx_idx0].Normal.Y, object.Vertices[vtx_idx0].Normal.Z);
                vec3 n_1(object.Vertices[vtx_idx1].Normal.X, object.Vertices[vtx_idx1].Normal.Y, object.Vertices[vtx_idx1].Normal.Z);
                vec3 n_2(object.Vertices[vtx_idx2].Normal.X, object.Vertices[vtx_idx2].Normal.Y, object.Vertices[vtx_idx2].Normal.Z);

                vec3 t_0(object.Vertices[vtx_idx0].TextureCoordinate.X, object.Vertices[vtx_idx0].TextureCoordinate.Y, 0.0f);
                vec3 t_1(object.Vertices[vtx_idx1].TextureCoordinate.X, object.Vertices[vtx_idx1].TextureCoordinate.Y, 0.0f);
                vec3 t_2(object.Vertices[vtx_idx2].TextureCoordinate.X, object.Vertices[vtx_idx2].TextureCoordinate.Y, 0.0f);

                triangle t0(vtx_0, vtx_1, vtx_2, n_0, n_1, n_2, t_0, t_1, t_2, id, sum(n_0, n_1, n_2));

                faces.push_back(t0);
            }

            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            vec3 rand_color(dist(mt), dist(mt), dist(mt));

            material new_material(id, rand_color, 0.0f);
            mesh new_mesh(faces, min, max, id);

            id++;

            scene.push_back(new_mesh);
            materials.push_back(new_material);
        }
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Scene loaded in " << elapsed.count() << " seconds" << std::endl;
}