#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>

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
const int samples = 32;
const int nee_samples = 8;
int bounces[] = { 4, 3, 10 };
float variance_threshold = 0.001;


vec3 cast_ray(const ray& r, vec3 color, float& u, float& v, std::vector<material>& mats, RTCScene& g_scene, std::vector<light>& lights, int depth[])
{   
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    float near_clipping = 0.0f;

    if(depth[0] == 3) near_clipping = 0.0f;

    RTCRayHit rayhit;
    rayhit.ray.org_x = r.origin().x;
    rayhit.ray.org_y = r.origin().y;
    rayhit.ray.org_z = r.origin().z;
    rayhit.ray.dir_x = r.direction().x;
    rayhit.ray.dir_y = r.direction().y;
    rayhit.ray.dir_z = r.direction().z;
    rayhit.ray.tnear = near_clipping;
    rayhit.ray.tfar = std::numeric_limits<float>::infinity();
    rayhit.ray.mask = -1;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

    vec3 new_color(0.0f);

    rtcIntersect1(g_scene, &context, &rayhit);

    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
    {
        int hit_mat_id = rayhit.hit.geomID;

        float hit_albedo = 1.0f;

        new_color = mats[hit_mat_id].clr;
        float hit_roughness = mats[hit_mat_id].roughness;
        float hit_refraction = mats[hit_mat_id].refraction_roughness;

        vec3 hit_normal = vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z).normalize();

        
        if (mats[hit_mat_id].islight)
        {
            float d = dot(r.direction(), mats[hit_mat_id].normal);
            if (d < 0 && depth[0] == 3) return mats[hit_mat_id].clr;
        }
        
        vec3 hit_pos = vec3(rayhit.ray.org_x, rayhit.ray.org_y, rayhit.ray.org_z) + rayhit.ray.tfar * vec3(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z);
        
        // reflection
        if (hit_roughness < 1.0)
        {
            if (depth[1] == 0) return vec3(0.f);
         
            vec3 new_ray_dir;
            new_ray_dir = reflect(r.direction(), hit_normal, hit_roughness);
            ray new_ray(hit_pos + hit_normal * 0.001f, new_ray_dir);

            vec3 reflection_color(1.0f, 0.874f, 0.0f);

            int new_depth[] = { depth[0], depth[1] - 1, depth[2] };

            color = cast_ray(new_ray, color, u, v, mats, g_scene, lights, new_depth) * reflection_color;
        }

        // refraction
        else if (hit_refraction > 0.0)
        {
            if (depth[2] == 0) return vec3(0.f);

            vec3 new_ray_dir;
            float offset;
            if (dot(r.direction(), hit_normal) > 0) offset = 0.001f;
            if (dot(r.direction(), hit_normal) < 0) offset = -0.001f;

            vec3 random_refrac = generate_random_vector(-0.025f, 0.025f);

            new_ray_dir = refract(r.direction(), hit_normal, 1.7f) + (random_refrac * 1);
            ray new_ray(hit_pos + hit_normal * offset, new_ray_dir);

            vec3 refraction_color(1.0f, 1.0f, 1.0f);

            int new_depth[] = { depth[0], depth[1], depth[2] - 1 };

            color = cast_ray(new_ray, color, u, v, mats, g_scene, lights, new_depth) * refraction_color;
        }
        
        // direct lighting
        else
        {
            if (depth[0] == 0) return vec3(0.f);

            for (auto light : lights)
            {
                for (int i = 0; i < nee_samples; i++)
                {
                    vec3 area_sample_position(0.0f);
                    vec3 ray_dir = return_raydir(light, hit_pos, hit_normal, area_sample_position);

                    ray new_ray(hit_pos, ray_dir);

                    float distance = 10000.0f;
                    float area_shadow = 1.0f;

                    if (light.type == 0) distance = dist(hit_pos, light.position) - 0.001f;

                    if (light.type == 2)
                    {
                        if (dot(ray_dir, light.orientation) > 0) continue;
                        vec3 pos = vec3(light.v0 + light.v1 + light.v2 + light.v3) / 4;
                        distance = dist(hit_pos, area_sample_position) - 0.001f;
                        float d = dot(light.orientation, ray_dir);
                        area_shadow = -d;
                    }

                    RTCRay shadow;
                    shadow.org_x = new_ray.origin().x;
                    shadow.org_y = new_ray.origin().y;
                    shadow.org_z = new_ray.origin().z;
                    shadow.dir_x = new_ray.direction().x;
                    shadow.dir_y = new_ray.direction().y;
                    shadow.dir_z = new_ray.direction().z;
                    shadow.tnear = 0.001f;
                    shadow.tfar = distance;
                    shadow.mask = -1;
                    shadow.flags = 0;

                    rtcOccluded1(g_scene, &context, &shadow);

                    if (shadow.tfar > 0.0f)
                    {
                        // lambert
                        color += new_color * (1.0f / float(M_PI)) * return_light_int(light, distance) * std::max(0.f, dot(hit_normal, ray_dir)) / nee_samples;

                        // oren-nayar
                    }
                }
                
            }
            
            // indirect lighting
            vec3 rand_ray_dir = random_ray_in_hemisphere(hit_normal);
            ray random_ray(hit_pos + hit_normal * 0.001f, rand_ray_dir);

            int new_depth[] = { depth[0] - 1, depth[1], depth[2] };

            color += cast_ray(random_ray, color, u, v, mats, g_scene, lights, new_depth) * std::max(0.f, dot(hit_normal, rand_ray_dir)) * new_color;
        }
    }

    return color;
}


static void render(tile* cur_tile, int xstart, int xend, int ystart, int yend, RTCScene g_scene, std::vector<material>& mats, std::vector<light>& lights)
{
    float fov = 43;
    float scale = tan(deg2rad(fov * 0.5));
    float imageAspectRatio = xres / (float)yres;

    //vec3 campos(14.3f, 3.7f, 4.2f);
    //vec3 aim(-2.2, 4.0f, -2.5);  //pos pixar
    vec3 campos(0.f, 7.5f, 27.0f);
    vec3 aim(0.f, 7.5f, 0.f);
    
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

                col += (cast_ray(ray, col, u, v, mats, g_scene, lights, bounces)) / samples;
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
    const char* filename = "D:/GenepiRender/Renders/test.png";
    const char* path = "D:/GenepiRender/Models/statue.obj";
    const int channels = 3; //rbg

    OIIO::ImageSpec spec(xres, yres, channels, OIIO::TypeDesc::FLOAT);

    OIIO::ImageBuf buffer(spec);

    OIIO::ROI roi(0, xres, 0, yres, 0, 3);

    std::vector<tile> tiles = generate_tiles(tile_number, xres, yres, 3);

    std::vector<light> lights;

    light dir_light(1, 1.5f, vec3(1.0f, 1.0f, 1.0f), vec3(0.5f, -0.5f, -1.0f), 0.5f);
    light dir_light2(1, 1.5f, vec3(1.0f, 0.45f, 0.07f), vec3(0.5f, -0.5f, -1.0f), 2.0f);

    //light square_light(2, false, 250.0f, vec3(0.6f, 0.8f, 0.9f), vec3(-9.1f, 3.25f, -3.5f), 10.0f, 5.0f, vec3(1, 0, 0));
    //light square_light2(2, false, 275.0f, vec3(0.6f, 0.8f, 0.9f), vec3(-9.1f, 3.25f, 3.0f), 10.0f, 5.0f, vec3(1, 0, 0));
    //light square_bounce(2, false, 5.0f, vec3(1.0f, 0.45f, 0.07f), vec3(5.0f, 0.0f, -2.8f), 4.0f, 3.0f, vec3(0,0,1));
    //light square_bounce2(2, false, 5.0f, vec3(1.0f, 0.45f, 0.07f), vec3(-2.0f, 0.0f, -2.8f), 4.0f, 3.0f, vec3(0, 0, 1));

    light square(2, false, 50.0f, vec3(0.65f, 0.85f, 0.96f), vec3(-7.5f, 0.0f, 10.0f), 15.0f, 15.0f, vec3(0, 0, -1));
    lights.push_back(square);

    //lights.push_back(square_light);
    //lights.push_back(square_light2);
    //lights.push_back(square_bounce);
    //lights.push_back(square_bounce2);

    lights.push_back(dir_light);
    lights.push_back(dir_light2);

    RTCDevice g_device = initializeDevice();
    RTCScene g_scene = rtcNewScene(g_device);

    rtcSetSceneBuildQuality(g_scene, RTC_BUILD_QUALITY_HIGH);
    rtcSetSceneFlags(g_scene, RTC_SCENE_FLAG_DYNAMIC | RTC_SCENE_FLAG_ROBUST);

    std::vector<material> materials;
    std::vector<light> lights2;

    load_scene(materials, lights, g_scene, g_device, path);
    rtcCommitScene(g_scene);

    

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