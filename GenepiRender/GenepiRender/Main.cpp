#include <OpenImageIO/imagebuf.h>
#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <chrono>
#include "objloader.h"
#include "ray.h"
#include "triangle.h"
#include "camera.h"
#include "matrix.h"
#include "bvh.h"


const int xres = 200;
const int yres = 100;
const int tiles = 8;
float infinity = std::numeric_limits<float>::max();


inline float deg2rad(const float& deg)
{
    return deg * M_PI / 180;
}


std::vector<triangle> scene(int count)
{
    std::vector<triangle> tris;

    for (int i = 0; i < count; i++)
    {
        std::random_device rd;
        std::random_device rd2;
        std::random_device rd3;
        std::random_device rd4;
        std::mt19937 mt(rd());
        std::mt19937 mt2(rd2());
        std::mt19937 mt3(rd3());
        std::mt19937 mt4(rd4());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        std::uniform_real_distribution<float> dist2(-3.0f, 3.0f);
        std::uniform_real_distribution<float> dist3(-3.0f, 3.0f);
        std::uniform_real_distribution<float> dist4(-10.0f, -5.0f);
        /*vec3 v0(dist(mt), dist(mt), dist2(mt2));
        vec3 v1(dist(mt), dist(mt), dist2(mt2));
        vec3 v2(dist(mt), dist(mt), dist2(mt2));*/
        vec3 rand_color(dist(mt), dist(mt), dist(mt));

        float x = dist2(mt2);
        float y = dist3(mt3);
        float z = dist4(mt4);

        vec3 v0(-1.0f + i, -1.0f, -5.0f - i);
        vec3 v1(1.0f + i, -1.0f, -5.0f - i);
        vec3 v2(0.0f + i, 1.0f, -5.0f - i);

        triangle t0(vec3(x, y, z), rand_color);

        tris.push_back(t0);
    }

    return tris;
}

std::vector<triangle> mesh(vec3& min, vec3& max)
{
    std::cout << "Loading mesh...." << std::endl;
    auto start = std::chrono::system_clock::now();

    objl::Loader loader;

    float min_x = 0.0, max_x = 0.0, min_y = 0.0, max_y = 0.0, min_z = 0.0, max_z = 0.0;

    std::vector<triangle> faces;

    bool loadout = loader.LoadFile("D:/GenepiRender/Models/dino.obj");

    if (loadout)
    {
        objl::Mesh mesh = loader.LoadedMeshes[0];
        //compute mins mand maxs for the bbox
        for (int j = 0; j < mesh.Vertices.size(); j++)
        {
            float px = mesh.Vertices[j].Position.X;
            float py = mesh.Vertices[j].Position.Y;
            float pz = mesh.Vertices[j].Position.Z;

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
        for (int i = 0; i < mesh.Indices.size(); i += 3)
        {
            int vtx_idx0 = mesh.Indices[i];
            int vtx_idx1 = mesh.Indices[i + 1];
            int vtx_idx2 = mesh.Indices[i + 2];

            vec3 vtx_0(mesh.Vertices[vtx_idx0].Position.X, mesh.Vertices[vtx_idx0].Position.Y, mesh.Vertices[vtx_idx0].Position.Z);
            vec3 vtx_1(mesh.Vertices[vtx_idx1].Position.X, mesh.Vertices[vtx_idx1].Position.Y, mesh.Vertices[vtx_idx1].Position.Z);
            vec3 vtx_2(mesh.Vertices[vtx_idx2].Position.X, mesh.Vertices[vtx_idx2].Position.Y, mesh.Vertices[vtx_idx2].Position.Z);

            vec3 n_0(mesh.Vertices[vtx_idx0].Normal.X, mesh.Vertices[vtx_idx0].Normal.Y, mesh.Vertices[vtx_idx0].Normal.Z);
            vec3 n_1(mesh.Vertices[vtx_idx1].Normal.X, mesh.Vertices[vtx_idx1].Normal.Y, mesh.Vertices[vtx_idx1].Normal.Z);
            vec3 n_2(mesh.Vertices[vtx_idx2].Normal.X, mesh.Vertices[vtx_idx2].Normal.Y, mesh.Vertices[vtx_idx2].Normal.Z);

            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            vec3 rand_color(dist(mt), dist(mt), dist(mt));

            triangle t0(vtx_0, vtx_1, vtx_2, sum(n_0, n_1, n_2));
            
            faces.push_back(t0);
        }
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Mesh loaded in " << elapsed.count() << " seconds" << std::endl;

    return faces;
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


inline bool node::tree_intersect(ray r)
{
    if (this->bbox_intersect(r))
    {
        if (this->children[0] == nullptr)
        {
            std::cout << "1" << std::endl;
            return true;
        }
        else
        {
            for (auto child : this->children)
            {
                if (child == nullptr) continue;
                child->tree_intersect(r);
            }
        }
    }
}


//vec3 color(const ray& r, std::vector<triangle>& tris, vec3& min, vec3& max)
//{
//    node bound(min, max);
//    vec3 color(0.0f, 0.0f, 0.0f);
//    const triangle* hit = nullptr;
//
//    if (bound.bbox_intersect(r))
//    {
//        //color = vec3(1.0f); //debug
//        float t;
//        hit = trace(r, tris, t, hit);
//        {
//            vec3 posT(r.origin() + r.direction() * t);
//            if (hit != nullptr) color = hit->color;
//        }
//    }
//    
//    return color;
//    
//}


int main()
{
    const char* filename = "C:/Users/augie/Desktop/test.png";
    const int channels = 3; //rbg

    OIIO::ImageSpec spec(xres, yres, channels, OIIO::TypeDesc::FLOAT);

    OIIO::ImageBuf buffer(spec);

    OIIO::ROI roi(0, xres, 0, yres, 0, 3);

    int z = 0;

    float pixel[3];

    float fov = 51.52;
    float scale = tan(deg2rad(fov * 0.5));
    float imageAspectRatio = xres / (float)yres;

    vec3 campos(-4.0, 2.0, 9.0);
    vec3 aim(-0.5, 0.0, 0.0);
    vec3 up(0, 1, 0);

    vec3 zAxis = ((campos - aim).normalize());
    vec3 xAxis = (cross(up, zAxis).normalize());
    vec3 yAxis = cross(zAxis, xAxis);

    Matrix44<float> cameraToWorld(xAxis.x, xAxis.y, xAxis.z, 0.0f, yAxis.x, yAxis.y, yAxis.z, 0.0f, zAxis.x, zAxis.y, zAxis.z, 0.0f, campos.x, campos.y, campos.z, 1.0f);

    Vec3f rayOriginWorld, rayPWorld;
    Vec3f campos2(0.0, 0.0, 0.0);

    camera cam(campos, aim, fov, imageAspectRatio);

    vec3 min(0.0f), max(0.0f);
    bool is_leaf = false;

    std::vector<triangle> tris = scene(50);
    std::vector<triangle> sphere = mesh(min, max);

    auto start_arts = std::chrono::system_clock::now();
    std::cout << "Building acceleration structure..." << std::endl;
    node tree(min, max, is_leaf);
    divide(&tree, 0, is_leaf);
    auto end_arts = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_arts = end_arts - start_arts;
    std::cout << "Acceleration structure built in " << elapsed_arts.count() << " seconds" << std::endl;

    int size = 0;

    //std::cout << size << std::endl;

    std::cout << "Starting render..." << std::endl;
    auto start = std::chrono::system_clock::now();

//#pragma omp parallel

    for (int y = 0; y < yres; y++)
    {
        for (int x = 0; x < xres; x++)
        {
            float x_ = (2 * (x + 0.5) / (float)xres - 1) * imageAspectRatio * scale;
            float y_ = (1 - 2 * (y + 0.5) / (float)yres) * scale;

            cameraToWorld.multVecMatrix(campos2, rayOriginWorld);
            cameraToWorld.multVecMatrix(Vec3f(x_, y_, -1), rayPWorld);
            Vec3f rayDir = rayPWorld - rayOriginWorld;
            rayDir.normalize();

            vec3 dir(rayDir.x, rayDir.y, rayDir.z);

            ray ray(campos, dir);

            vec3 col;

            if (tree.tree_intersect(ray))
            {
                col = vec3(1.0f, 1.0f, 1.0f);
                std::cout << "Hit !" << std::endl;
            }

            else
            {
                col = vec3(0.0f);
            }
            //
            //vec3 col = color(ray, sphere, min, max);

            pixel[0] = col.x;
            pixel[1] = col.y;
            pixel[2] = col.z;

            buffer.setpixel(x, y, z, pixel);
        }
    }

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Render time : " << elapsed.count() << " seconds" << std::endl;

    buffer.write(filename);
}