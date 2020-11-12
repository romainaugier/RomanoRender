#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>

#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <chrono>
#include <future>
#include <algorithm>
#include <math.h>


#include "objloader.h"
#include "ray.h"
#include "triangle.h"
#include "camera.h"
#include "matrix.h"
#include "tiles.h"
#include "scene.h"
#include "material.h"
#include "light.h"
#include "utils.h"


const int xres = 1000;
const int yres = 1000;
const int tile_number = 8;
const int samples = 4;
const int bounces = 3;
float variance_threshold = 0.001;


const triangle * trace(const ray& r, std::vector<triangle>& tris, float &t_near, const triangle *hit, float& u, float& v)
{
    float t;
    t_near = infinity;
    int id;
    int i = 0;
    float uTriangle, vTriangle;
    std::vector<triangle>::const_iterator iter = tris.begin();
    for (; iter != tris.end(); iter++)
    {
        t = infinity;
        if (intersect(r.origin(), r.direction(), tris[i], uTriangle, vTriangle, t) && t < t_near)
        {
            //std::cout << "hit" << std::endl;
            hit = &(*iter);
            t_near = t;
            u = uTriangle;
            v = vTriangle;

        }
        i++;
    }
    return(hit);
}


vec3 cast_ray(const ray& r, vec3 color, float& u, float& v, std::vector<material>& mats, RTCScene& g_scene, std::vector<point_light>& lights, int depth)
{
    if (depth == 0) return vec3(0.f);
    
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    RTCRayHit rayhit;
    rayhit.ray.org_x = r.origin().x;
    rayhit.ray.org_y = r.origin().y;
    rayhit.ray.org_z = r.origin().z;
    rayhit.ray.dir_x = r.direction().x;
    rayhit.ray.dir_y = r.direction().y;
    rayhit.ray.dir_z = r.direction().z;
    rayhit.ray.tnear = 0;
    rayhit.ray.tfar = std::numeric_limits<float>::infinity();
    rayhit.ray.mask = -1;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

    //hit_tris = scene_intersect(r, trees, id);
    // intersect ray with scene
    
    rtcIntersect1(g_scene, &context, &rayhit);

    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
    {
        std::cout << rayhit.hit.geomID << "\n";
        return vec3(1.f);
    }
    else return color;
        /*
        hit = trace(r, hit_tris, t, hit, u, v);

        if (hit != nullptr)
        {
            hit_mat_id = hit->mat_id;
            //hit_pos = (1 - u - v) * hit->vtx0 + u * hit->vtx1 + v * hit->vtx2;
            hit_pos = r.origin() + r.direction() * t;
            hit_normal = (1 - u - v) * hit->n0 + u * hit->n1 + v * hit->n2;
            hit_geometric_normal = sum(hit->n0, hit->n1, hit->n2);
            hit_tex = (1 - u - v) * hit->t0 + u * hit->t1 + v * hit->t2;
            hit_roughness = mats[hit_mat_id].roughness;
            hit_refraction = mats[hit_mat_id].refraction_roughness;
            float bias = 0.009;
            new_color = mats[hit_mat_id].clr;


            
            if (mats[hit_mat_id].has_clr_map)
            {
                int xmod = modulo(hit_tex.x) * (mats[hit_mat_id].clr_buf_w / 2);
                int ymod = modulo(hit_tex.y) * mats[hit_mat_id].clr_buf_h;
                new_color = get_tex_pixel(mats[hit_mat_id].clr_buffer, xmod, ymod) * 1.5;
            }
            else
            {
                new_color = mats[hit_mat_id].clr;
            }

            if (mats[hit_mat_id].has_normal_map)
            {
                int xmod = modulo(hit_tex.x) * (mats[hit_mat_id].clr_buf_w / 2);
                int ymod = modulo(hit_tex.y) * mats[hit_mat_id].clr_buf_h;
                hit_normal = get_tex_pixel(mats[hit_mat_id].normal_buffer, xmod, ymod) * 1.5;
            }

            
            if (hit_roughness < 1.0)
            {
                vec3 new_ray_dir;
                new_ray_dir = reflect(r.direction(), hit_normal, hit_roughness);
                ray new_ray(hit_pos + hit_normal * 0.01, new_ray_dir);
                color = cast_ray(new_ray, color, u, v, mats, trees, lights, depth - 1);
            }
            if (hit_refraction > 0.0)
            {
                //depth = depth--;
                vec3 new_ray_dir;
                new_ray_dir = refract(r.direction(), hit_normal, 1.7f);

                //color = vec3(0.f);

                ray new_ray(hit_pos - hit_normal * 0.5f, new_ray_dir);
                color = cast_ray(new_ray, color, u, v, mats, trees, lights, depth - 1);
            }

            //direct lighting
            else
            { 

                if (hit_mat_id == 1)
                {
                    float scale = 40.0;
                    float s = (modulo(hit_tex.y * scale) < 0.5) ^ (modulo(hit_tex.x * scale) < 0.5);
                    //new_color = vec3(s);
                }

                for (auto pt_light : lights)
                {
                    float has_hit = 1.0;
                    vec3 ray_dir = pt_light.position - hit_pos;
                    ray new_ray(hit_pos + hit_normal * bias, ray_dir);

                    float distance = dist(hit_pos, pt_light.position);

                    if (dot(ray_dir, hit_normal) < 0)
                    {
                        //has_hit = 0.0;
                    }

                    else
                    {
                        new_hit_tris = scene_intersect(new_ray, trees, id);

                        if (new_hit_tris.size() > 0)
                        {
                            float epsilon = 0.009;

                            new_hit = trace(new_ray, new_hit_tris, t1, new_hit, u, v);

                            if (new_hit != nullptr && t1 > epsilon && t1 <= distance)
                            {
                                has_hit = 0.0;
                            }
                        }
                    }

                    color += new_color * has_hit * pt_light.light_intensity(distance) * std::max(0.f, dot(hit_normal, ray_dir));
                }
            }

            //indirect lighting
            double r1 = generate_random_float(0.0, 1.0);
            double r2 = generate_random_float(0.0, 1.0);

            vec3 rand_dir_local(cos(2 * M_PI * r1) * sqrt(1 - r2), sin(2 * M_PI * r1) * sqrt(1 - r2), sqrt(1 - r1));
            vec3 rand(generate_random_float(0.0, 1.0) - 0.5, generate_random_float(0.0, 1.0) - 0.5, generate_random_float(0.0, 1.0) - 0.5);
        
            vec3 tan1 = cross(hit_normal, rand);
            vec3 tan2 = cross(tan1.normalize(), hit_normal);

            vec3 rand_ray_dir = rand_dir_local.z * hit_normal + rand_dir_local.x * tan1 + rand_dir_local.y  * tan2;
            ray random_ray(hit_pos + hit_normal * bias, rand_ray_dir);
            
            color += cast_ray(random_ray, color, u, v, mats, trees, lights, depth - 1) * new_color;
            
        }
    }*/
}


static void render(tile* cur_tile, int xstart, int xend, int ystart, int yend, RTCScene g_scene, std::vector<material>& mats, std::vector<point_light>& lights)
{
    float fov = 50;
    float scale = tan(deg2rad(fov * 0.5));
    float imageAspectRatio = xres / (float)yres;

    int depth = bounces;

    //vec3 campos(20, 5.f, 7.5);
    //vec3 aim(-2.2, 5.5f, -2.5);  //pos pixar
    vec3 campos(0.f, 7.5, 25.f);
    vec3 aim(0.f, 7.5f, 0.f);
    //vec3 campos(0.0, 0.0, 3.0);
    //vec3 aim(0.0, 0.0, 0.0);
    
    vec3 up(0, 1, 0);
    
    vec3 zAxis = ((campos - aim).normalize());
    vec3 xAxis = (cross(up, zAxis).normalize());
    vec3 yAxis = cross(zAxis, xAxis);
    
    Matrix44<float> cameraToWorld(xAxis.x, xAxis.y, xAxis.z, 0.0f, yAxis.x, yAxis.y, yAxis.z, 0.0f, zAxis.x, zAxis.y, zAxis.z, 0.0f, campos.x, campos.y, campos.z, 1.0f);
    
    Vec3f rayOriginWorld, rayPWorld;
    Vec3f campos2(0.0, 0.0, 0.0);

    float pixel[3];

    auto start_tile = std::chrono::system_clock::now();

    for (int y = ystart; y < yend; y++)
        {
        for (int x = xstart; x < xend; x++)
        {
            vec3 col(0.f);
            float t;
            float u, v;


            for (int s = 0; s < samples; s++)
            {
                //adaptive sampling
                //if (variance < variance_threshold) continue;

                //AA Box-Muller
                int aa_samples = 1;
                float r1 = generate_random_float(0.0, 1.0);
                float r2 = generate_random_float(0.0, 1.0);
                float dx = sqrt(-0.5 * log(r1)) * cos(2 * M_PI * r2);
                float dy = sqrt(-0.5 * log(r1)) * sin(2 * M_PI * r2);

                float x_ = (2 * (x + 0.5 + dx) / (float)xres - 1) * imageAspectRatio * scale;
                float y_ = (1 - 2 * (y + 0.5 + dy) / (float)yres) * scale;

                cameraToWorld.multVecMatrix(campos2, rayOriginWorld);
                cameraToWorld.multVecMatrix(Vec3f(x_, y_, -1), rayPWorld);
                Vec3f rayDir = rayPWorld - rayOriginWorld;
                rayDir.normalize();

                vec3 dir(rayDir.x, rayDir.y, rayDir.z);
                ray ray(campos, dir);

                col += cast_ray(ray, col, u, v, mats, g_scene, lights, depth) / samples;
            }

            pixel[0] = pow(col.x, 1.0 / 2.2);
            pixel[1] = pow(col.y, 1.0 / 2.2);
            pixel[2] = pow(col.z, 1.0 / 2.2);
        
            set_tile_pixel(*cur_tile, col.x, col.y, col.z);
            }
        }

    auto end_tile = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_tile = end_tile - start_tile;
    //std::cout << "Tile " << cur_tile->id << " rendered in " << elapsed_tile.count() << " seconds" << "\n";

}


int main()
{
    const char* filename = "C:/Users/augie/Desktop/test.png";
    const char* path = "D:/GenepiRender/Models/triangle.obj";
    const int channels = 3; //rbg

    OIIO::ImageSpec spec(xres, yres, channels, OIIO::TypeDesc::FLOAT);

    OIIO::ImageBuf buffer(spec);

    OIIO::ROI roi(0, xres, 0, yres, 0, 3);

    std::vector<tile> tiles = generate_tiles(tile_number, xres, yres, 3);

    std::vector<point_light> lights;

    point_light pt_light1(17.5, vec3(1.0f, 1.0f, 1.0f), vec3(0.f, 10.0f, 0.f));
    point_light pt_light2(10.0, vec3(0.6f, 0.6f, 1.0f), vec3(-4.0f, 10.f, 0.f));
    point_light pt_light3(10.0, vec3(1.0f, 1.0f, 1.0f), vec3(4.0f, 10.f, 0.f));
    point_light pt_light4(100.0, vec3(1.0f, 0.25f, 0.15f), vec3(3.0f, 6.0f, 0.0f));


    lights.push_back(pt_light1);
    lights.push_back(pt_light2);
    lights.push_back(pt_light3);
    //lights.push_back(pt_light4);

    RTCDevice g_device = initializeDevice();
    RTCScene g_scene = rtcNewScene(g_device);

    std::vector<mesh> scene;
    std::vector<material> materials;
    load_scene(scene, materials, g_scene, g_device, path);
    rtcCommitScene(g_scene);

    std::cout << g_scene << "\n";

    int size = 0;

    std::vector<std::future<void>> futures;

    std::cout << "Starting render..." << std::endl;
    std::cout << "=================================" << std::endl;
    auto start = std::chrono::system_clock::now();

    float progress = 100.0 / (tile_number * tile_number);

    for (auto& tile : tiles)
    {
        futures.push_back(std::async(std::launch::async, render, &tile, tile.xstart, tile.xend, tile.ystart, tile.yend, g_scene, materials, lights));
        //render(&tile, tile.xstart, tile.xend, tile.ystart, tile.yend, g_scene, materials, lights);
    }


    for(int i = 0; i < futures.size(); i++)
    {
        futures[i].wait();
        float p = progress* (i + 1);
        std::cout << "progress : " << std::floor(p) << "%                        " << "\r";
    }

    std::cout << "\n";

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
    std::cout << "=================================" << std::endl;
    std::cout << "Render time : " << elapsed.count() << " seconds" << std::endl;

    buffer.write(filename);
}