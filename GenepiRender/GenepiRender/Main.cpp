#include <OpenImageIO/imagebuf.h>
#include "ray.h"
#include "triangle.h"
#include <iostream>
#include <vector>
#include <random>
#include <limits>


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
        std::mt19937 mt(rd());
        std::mt19937 mt2(rd2());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        std::uniform_real_distribution<float> dist2(-10.0f, -3.0f);
        /*vec3 v0(dist(mt), dist(mt), dist2(mt2));
        vec3 v1(dist(mt), dist(mt), dist2(mt2));
        vec3 v2(dist(mt), dist(mt), dist2(mt2));*/
        vec3 rand_color(dist(mt), dist(mt), dist(mt));

        vec3 v0(-1.0f + i, -1.0f, -5.0f - i);
        vec3 v1(1.0f + i, -1.0f, -5.0f - i);
        vec3 v2(0.0f + i, 1.0f, -5.0f - i);

        triangle t0(v0, v1, v2, rand_color);

        tris.push_back(t0);
    }

    return tris;
}


int trace(const ray& r, std::vector<triangle>& tris)
{
    float u = 0.0f;
    float v = 0.0f;
    float t, t_near;
    t_near = infinity;
    int id;

    for (int i = 0; i < tris.size(); i++)
    {
        t = infinity;
        if (intersect(r.origin(), r.direction(), tris[i], u, v, t) && t < t_near)
        {
            t_near = t;
            id = i + 1;
            return id;
        }
        else return 0;
    }
}


vec3 color(const ray& r, std::vector<triangle>& tris)
{
    int id = trace(r, tris);
    if (id == 0) return vec3(0.0f, 0.0f, 0.0f);
    else
    {
        vec3 color(tris[id].color);
        return color;
    }
}


int main()
{
    const char* filename = "D:/test.png";
    const int channels = 3; //rbg

    OIIO::ImageSpec spec(xres, yres, channels, OIIO::TypeDesc::FLOAT);

    OIIO::ImageBuf buffer(spec);

    OIIO::ROI roi(0, xres, 0, yres, 0, 3);

    int z = 0;

    float pixel[3];

    float fov = 75;
    float scale = tan(deg2rad(fov * 0.5));

    std::vector<triangle> tris = scene(3);

    for (int y = 0; y < yres; y++)
    {
        for (int x = 0; x < xres; x++)
        {
            float x_ = (2 * (x + 0.5) / float(xres) - 1) * float(xres / yres) * scale;
            float y_ = (1 - 2 * (y + 0.5) / float(yres)) * scale;

            vec3 dir(x_, y_, -1);
            vec3 origin(0.0f, 0.0f, 0.0f);

            ray ray(origin, dir);
            vec3 col = color(ray, tris);

            pixel[0] = col.x;
            pixel[1] = col.y;
            pixel[2] = col.z;

            //QRgb rgb = qRgb(r * 255, g * 255, b * 255);
            //background.setPixel(x, y, rgb);

            buffer.setpixel(x, y, z, pixel);
        }
    }


    buffer.write(filename);
}