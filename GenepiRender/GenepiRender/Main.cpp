#include <OpenImageIO/imagebuf.h>
#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <chrono>
#include <future>
#include "objloader.h"
#include "ray.h"
#include "triangle.h"
#include "camera.h"
#include "matrix.h"
#include "bvh.h"
#include "tiles.h"
#include "scene.h"


const int xres = 1000;
const int yres = 1000;
float infinity = std::numeric_limits<float>::max();


inline float deg2rad(const float& deg)
{
    return deg * M_PI / 180;
}


void load_scene(std::vector<mesh>& scene)
{
    std::cout << "Loading scene...." << std::endl;
    auto start = std::chrono::system_clock::now();

    objl::Loader loader;

    float min_x = 0.0, max_x = 0.0, min_y = 0.0, max_y = 0.0, min_z = 0.0, max_z = 0.0;

    bool loadout = loader.LoadFile("D:/GenepiRender/Models/scene.obj");

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

                std::random_device rd;
                std::mt19937 mt(rd());
                std::uniform_real_distribution<float> dist(0.0f, 1.0f);
                vec3 rand_color(dist(mt), dist(mt), dist(mt));

                triangle t0(vtx_0, vtx_1, vtx_2, sum(n_0, n_1, n_2));

                faces.push_back(t0);
            }

            mesh new_mesh(faces, min, max, id);
            id++;

            scene.push_back(new_mesh);
        }   
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Scene loaded in " << elapsed.count() << " seconds" << std::endl;
}


const triangle * trace(const ray& r, std::vector<triangle>& tris, float &t_near, const triangle *hit)
{
    float u = 0.0f;
    float v = 0.0f;
    float t;
    t_near = infinity;
    int id;
    int i = 0;
    std::vector<triangle>::const_iterator iter = tris.begin();
    for (; iter != tris.end(); iter++)
    {
        t = infinity;
        if (intersect(r.origin(), r.direction(), tris[i], u, v, t) && t < t_near)
        {
            //std::cout << "hit" << std::endl;
            hit = &(*iter);
            t_near = t;
        }
        i++;
    }
    return(hit);
}


vec3 color(const ray& r, std::vector<triangle>& tris, vec3 color)
{
    const triangle* hit = nullptr;

    float t;
    hit = trace(r, tris, t, hit);
    vec3 posT(r.origin() + r.direction() * t);
    if (hit != nullptr) color = hit->color;
    
    return color;    
}


std::vector<tile> generate_tiles(const int& number, const int& xres, const int &yres, const int& channels)
{
    std::vector<tile> tiles;

    for (int y = 0; y < number; y++)
    {
        int miny = yres / number * y;
        int maxy = yres / number * (y + 1);

        for (int x = 0; x < number; x++)
        {
            int minx = xres / number * x;
            int maxx = xres / number * (x + 1);
            
            tile new_tile(minx, maxx, miny, maxy, channels);
            tiles.push_back(new_tile);
        }
    }

    return tiles;
}


static void render(tile* cur_tile, int xstart, int xend, int ystart, int yend, std::vector<node>& trees)
{
    float fov = 50;
    float scale = tan(deg2rad(fov * 0.5));
    float imageAspectRatio = xres / (float)yres;

    vec3 campos(0.0, 4.9, 15.0);
    vec3 aim(0.0, 4.9, 0.0);
    vec3 up(0, 1, 0);
    
    vec3 zAxis = ((campos - aim).normalize());
    vec3 xAxis = (cross(up, zAxis).normalize());
    vec3 yAxis = cross(zAxis, xAxis);
    
    Matrix44<float> cameraToWorld(xAxis.x, xAxis.y, xAxis.z, 0.0f, yAxis.x, yAxis.y, yAxis.z, 0.0f, zAxis.x, zAxis.y, zAxis.z, 0.0f, campos.x, campos.y, campos.z, 1.0f);
    
    Vec3f rayOriginWorld, rayPWorld;
    Vec3f campos2(0.0, 0.0, 0.0);

    float pixel[3];


    for (int y = ystart; y < yend; y++)
        {
            for (int x = xstart; x < xend; x++)
            {
                float x_ = (2 * (x + 0.5) / (float)xres - 1) * imageAspectRatio * scale;
                float y_ = (1 - 2 * (y + 0.5) / (float)yres) * scale;
        
                cameraToWorld.multVecMatrix(campos2, rayOriginWorld);
                cameraToWorld.multVecMatrix(Vec3f(x_, y_, -1), rayPWorld);
                Vec3f rayDir = rayPWorld - rayOriginWorld;
                rayDir.normalize();
        
                vec3 dir(rayDir.x, rayDir.y, rayDir.z);
        
                ray ray(campos, dir);
        
                vec3 col(0.f);
                float t;
                std::vector<triangle> hit;

                for (auto tree : trees)
                {
                    hit = tree.tree_intersect(ray, t);
                }
        
                if(hit.size() > 0)
                    col = color(ray, hit, col);
        
                //col = color(ray, sphere, min, max);
        
                pixel[0] = col.x;
                pixel[1] = col.y;
                pixel[2] = col.z;
        
                set_tile_pixel(*cur_tile, col.x, col.y, col.z);
            }
        }

}


int main()
{
    const char* filename = "C:/Users/augie/Desktop/test.png";
    const int channels = 3; //rbg

    OIIO::ImageSpec spec(xres, yres, channels, OIIO::TypeDesc::FLOAT);

    OIIO::ImageBuf buffer(spec);

    OIIO::ROI roi(0, xres, 0, yres, 0, 3);

    const int tile_number = 8;
    std::vector<tile> tiles = generate_tiles(tile_number, xres, yres, 3);

    std::vector<mesh> scene;
    load_scene(scene);
    
    auto start_arts = std::chrono::system_clock::now();
    std::cout << "Building acceleration structure..." << std::endl;

    std::vector<node> scene_tree;

    for (auto& obj : scene)
    {
        node tree(obj.min, obj.max, vec3(1.f));

        if (obj.tris.size() > 100)
            divide(&tree, 8, 0);

        else divide(&tree, 1, 10);

        push_triangles(&tree, obj.tris);

        scene_tree.push_back(tree);
    }

    auto end_arts = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_arts = end_arts - start_arts;
    std::cout << "Acceleration structure built in " << elapsed_arts.count() << " seconds" << std::endl;

    int size = 0;

    std::vector<std::future<void>> futures;

    std::cout << "Starting render..." << std::endl;
    auto start = std::chrono::system_clock::now();

    for (auto& tile : tiles)
    {
        futures.push_back(std::async(std::launch::async, render, &tile, tile.xstart, tile.xend, tile.ystart, tile.yend, scene_tree));
        //render(&tile, tile.xstart, tile.xend, tile.ystart, tile.yend, tree);
    }

    for(int i = 0; i < futures.size(); i++)
    {
        futures[i].wait();
    }

    for (auto& tile : tiles)
    {
        float pixels[3];
        int index = 0;
        for (int j = tile.ystart; j < tile.yend; j++)
        {
            for (int i = tile.xstart; i < tile.xend; i++)
            {
                pixels[0] = tile.data[index];
                pixels[1] = tile.data[index + 1];
                pixels[2] = tile.data[index + 2];

                buffer.setpixel(i, j, 0, pixels);

                index += 3;
            }
        }
    }

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Render time : " << elapsed.count() << " seconds" << std::endl;

    buffer.write(filename);
}