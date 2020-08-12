#include <OpenImageIO/imagebuf.h>
#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <chrono>
#include "objloader.h"
#include "ray.h"
#include "triangle.h"


const int xres = 1280;
const int yres = 720;
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

std::vector<triangle> mesh()
{
    objl::Loader loader;

    std::vector<triangle> faces;

    bool loadout = loader.LoadFile("C:/Users/augie/Desktop/xyzdragon.obj");

    if (loadout)
    {
        objl::Mesh mesh = loader.LoadedMeshes[0];

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


vec3 color(const ray& r, std::vector<triangle>& tris)
{
    vec3 color(0.0f, 0.0f, 0.0f);
    const triangle* hit = nullptr;
    float t;
    hit = trace(r, tris, t, hit);
    {
        vec3 posT(r.origin() + r.direction() * t);
        if(hit != nullptr) color = hit->color;
    }
    
    return color;
    
}


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

    std::vector<triangle> tris = scene(50);
    std::vector<triangle> sphere = mesh();

    auto start = std::chrono::system_clock::now();

    for (int y = 0; y < yres; y++)
    {
        for (int x = 0; x < xres; x++)
        {
            float x_ = (2 * (x + 0.5) / float(xres) - 1) * float(xres / yres) * scale;
            float y_ = (1 - 2 * (y + 0.5) / float(yres)) * scale;

            vec3 dir(x_, y_, -1);
            vec3 origin(0.0f, 0.0f, 0.0f);

            ray ray(origin, dir);
            vec3 col = color(ray, sphere);

            pixel[0] = col.x;
            pixel[1] = col.y;
            pixel[2] = col.z;

            //QRgb rgb = qRgb(r * 255, g * 255, b * 255);
            //background.setPixel(x, y, rgb

            buffer.setpixel(x, y, z, pixel);
        }
    }

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Render time : " << elapsed.count() << " seconds" << std::endl;


    buffer.write(filename);
}