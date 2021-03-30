#include "render.h"


// funtion used to render a single pixel, used for progressive rendering
void render_p(int s, std::vector<vec2>& sampler, color_t* pixels, int x, int y, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat)
{
    const float scale = tan(deg2rad(cam.fov * 0.5));

    vec3 campos2(0.0, 0.0, 0.0);

    vec3 col(0.f);

    // aa Box-Muller
    vec2 d = sampler[s];
    float dx = sqrt(-0.5f * log(d.x)) * cos(2.0f * M_PI * d.y);
    float dy = sqrt(-0.5f * log(d.x)) * sin(2.0f * M_PI * d.y);

    float x_ = (2.0f * (x + dx) / (float)settings.xres - 1.0f) * cam.aspect * scale;
    float y_ = (1.0f - 2.0f * (y + dy) / (float)settings.yres) * scale;

    // dof
    vec3 rand = vec3(0.0f);
    if (cam.aperture > 0.0f) rand = (cam.aperture / 2.0f) * sample_unit_disk(s);
    vec3 sample_pos = vec3(x_, y_, -1.0f);

    // ray generation
    vec3 rayOriginWorld = transform(campos2, cam.transformation_matrix);
    vec3 rayPWorld = transform(sample_pos, cam.transformation_matrix);
    vec3 rayDir = vec3(rayPWorld.x, rayPWorld.y, rayPWorld.z) - vec3(rayOriginWorld.x, rayOriginWorld.y, rayOriginWorld.z);

    // depth of field
    vec3 dir = cam.pos + cam.focus_dist * rayDir.normalize();
    vec3 new_pos = cam.pos + rand;

    Ray ray(new_pos, (dir - new_pos).normalize());

    std::vector<int> light_path;

    if (settings.integrator == 0) col = pathtrace(s * x * y, sampler, ray, col, mats, settings, lights, bounces, light_path, samples, stat);
    else if (settings.integrator == 2) col = ambient_occlusion(s * x * y, sampler, ray, settings);
    else if (settings.integrator == 3) col = scene_viewer(ray, settings);

    //col = HableToneMap(col);

    if (col.x < 0.0f || col.y < 0.0f || col.z < 0.0f)
    {
        col.x = std::max(col.x, 0.0f);
        col.y = std::max(col.y, 0.0f);
        col.z = std::max(col.z, 0.0f);

    }

    pixels[x + y * settings.xres].R += col.x;
    pixels[x + y * settings.xres].G += col.y;
    pixels[x + y * settings.xres].B += col.z;
}


// funtion used to render a single pixel, used for progressive rendering
void render_p_fast(int s, std::vector<vec2>& sampler, color_t* pixels, int x, int y, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat)
{
    const float scale = tan(deg2rad(cam.fov * 0.5));

    vec3 campos2(0.0, 0.0, 0.0);

    vec3 col(0.f);

    // aa Box-Muller
    vec2 d = sampler[s];
    float dx = sqrt(-0.5f * log(d.x)) * cos(2.0f * M_PI * d.y);
    float dy = sqrt(-0.5f * log(d.x)) * sin(2.0f * M_PI * d.y);

    float x_ = (2.0f * (x + dx) / (float)settings.xres - 1.0f) * cam.aspect * scale;
    float y_ = (1.0f - 2.0f * (y + dy) / (float)settings.yres) * scale;

    // dof
    vec3 rand = vec3(0.0f);
    if (cam.aperture > 0.0f) rand = (cam.aperture / 2.0f) * sample_unit_disk(s);
    vec3 sample_pos = vec3(x_, y_, -1.0f);

    // ray generation
    vec3 rayOriginWorld = transform(campos2, cam.transformation_matrix);
    vec3 rayPWorld = transform(sample_pos, cam.transformation_matrix);
    vec3 rayDir = vec3(rayPWorld.x, rayPWorld.y, rayPWorld.z) - vec3(rayOriginWorld.x, rayOriginWorld.y, rayOriginWorld.z);

    // depth of field
    vec3 dir = cam.pos + cam.focus_dist * rayDir.normalize();
    vec3 new_pos = cam.pos + rand;

    Ray ray(new_pos, (dir - new_pos).normalize());

    std::vector<int> light_path;

    if (settings.integrator == 0) col = pathtrace(s * x * y, sampler, ray, col, mats, settings, lights, bounces, light_path, samples, stat);
    else if (settings.integrator == 2) col = ambient_occlusion(s * x * y, sampler, ray, settings);
    else if (settings.integrator == 3) col = scene_viewer(ray, settings);

    if (col.x < 0.0f || col.y < 0.0f || col.z < 0.0f)
    {
        col.x = std::max(col.x, 0.0f);
        col.y = std::max(col.y, 0.0f);
        col.z = std::max(col.z, 0.0f);

    }

    pixels[x + y * (settings.xres)].R += col.x;
    pixels[x + y * (settings.xres)].G += col.y;
    pixels[x + y * (settings.xres)].B += col.z;
}


// function used to render progressively to the screen
void progressive_render(int s, int* ids, int y, std::vector<std::vector<vec2>>& sampler, color_t*& pixels, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat)
{
    cam.fov = 2 * rad2deg(std::atan(36.0f / (2 * cam.focal_length)));

#pragma omp parallel for
    for (int z = y; z < std::min(y + 50, settings.yres); z++)
    {
        for (int x = 0; x < settings.xres; x++)
        {
            int id = ids[x + z * settings.xres];
            render_p(s + x + z, sampler[id], pixels, x, z, settings, cam, mats, lights, samples, bounces, stat);
        }
    }
}


void progressive_render_fast(int s, int* ids, std::vector<std::vector<vec2>>& sampler, color_t*& pixels, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat)
{
    cam.fov = 2 * rad2deg(std::atan(36.0f / (2 * cam.focal_length)));

#pragma omp parallel for
    for (int z = 0; z < settings.yres; z++)
    {
        for (int x = 0; x < settings.xres; x++)
        {
            int id = ids[x + z * settings.xres];
            render_p(s, sampler[id], pixels, x, z, settings, cam, mats, lights, samples, bounces, stat);
        }

    }
}

