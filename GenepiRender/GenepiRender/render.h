#pragma once
#include "include_all.h"

#ifndef RENDER
#define RENDER


// ray cast function used to render. Return a vec3 color
vec3 cast_ray(const ray& r, vec3 color, std::vector<material>& mats, RTCScene& g_scene, std::vector<light>& lights, int depth[], int samples[], stats& stat)
{
    stat.add_ray();

    if (depth[0] == 0 || depth[1] == 0 || depth[2] == 0) return vec3(0.0f);

    RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    float near_clipping = 0.0f;
    float far_clipping = 10000.0f;

    if (depth[0] == 3) near_clipping = 0.0f;

    RTCRayHit rayhit;
    rayhit.ray.org_x = r.origin().x;
    rayhit.ray.org_y = r.origin().y;
    rayhit.ray.org_z = r.origin().z;
    rayhit.ray.dir_x = r.direction().x;
    rayhit.ray.dir_y = r.direction().y;
    rayhit.ray.dir_z = r.direction().z;
    rayhit.ray.tnear = near_clipping;
    rayhit.ray.tfar = far_clipping;
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
        float hit_diff_roughness = mats[hit_mat_id].diffuse_roughness;
        float hit_roughness = std::max(0.005f, mats[hit_mat_id].roughness);
        float hit_refraction = mats[hit_mat_id].refraction;
        float hit_metallic = mats[hit_mat_id].metallic;
        float hit_specular = mats[hit_mat_id].specular;
        float hit_reflectance = mats[hit_mat_id].reflectance;
        vec3 hit_specular_color = mats[hit_mat_id].specular_color;

        vec3 hit_ior = mats[hit_mat_id].ior;
        vec3 hit_refl_color = mats[hit_mat_id].reflection_color;

        Vertex n;
        rtcInterpolate0(rtcGetGeometry(g_scene, rayhit.hit.geomID), rayhit.hit.primID, rayhit.hit.u, rayhit.hit.v, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &n.x, 3);

        vec3 hit_normal = vec3(n.x, n.y, n.z);
        
        if (mats[hit_mat_id].islight)
        {
            if (depth[2] < 10) return mats[hit_mat_id].clr * 2.0f;
            return mats[hit_mat_id].clr;
        }
        

        vec3 hit_pos = vec3(rayhit.ray.org_x, rayhit.ray.org_y, rayhit.ray.org_z) + rayhit.ray.tfar * vec3(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z);

        vec3 kd(1.0f);
        vec3 ks(0.5f);
        vec3 radiance(0.0f);
        vec3 specular(0.0f);
        vec3 refrac(0.0f);
        vec3 indirect(0.0f);

        vec3 ggx(0.0f);

        vec3 tan, bitan;
        createBasis(hit_normal, tan, bitan);

        vec3 wo, wi, wm;

        for (auto light : lights)
        {
            for (int i = 0; i < samples[1]; i++)
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

                float NdotL = std::max(0.f, dot(hit_normal, ray_dir));

                vec3 H = (ray_dir + -r.direction()).normalize();
                float NdotH = Saturate(dot(hit_normal, H));
                float LdotH = Saturate(dot(ray_dir, H));
                float NdotV = Saturate(dot(hit_normal, -r.direction()));

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

                worldToTangent(hit_normal, tan, bitan, ray_dir, -r.direction(), wo, wi, wm);

                float D = GGXNormalDistribution(NdotH, hit_roughness);
                float G = SchlickMaskingTerm(NdotL, NdotV, hit_roughness);
                vec3 F = SchlickFresnel(hit_ior, LdotH);
                
                rtcOccluded1(g_scene, &context, &shadow);

                if (shadow.tfar > 0.0f)
                {
                    ggx += (D * G * F / (4 * std::max(0.001f, NdotV))) / samples[1];
                    radiance += (return_light_int(light, ray_dir, distance) * NdotL * area_shadow) / samples[1];
                }
            }
        }

        /*
        // reflectance
        if (hit_reflectance > 0.0f)
        {
            vec3 H = GGXMicrofacet(hit_normal, hit_roughness).normalize();

            vec3 new_ray_dir = reflect(r.direction(), H);

            ray new_ray(hit_pos + hit_normal * 0.001f, new_ray_dir);

            int new_depth[] = { depth[0], depth[1] - 1, depth[2] };

            specular += cast_ray(new_ray, color, mats, g_scene, lights, new_depth, samples, stat) * mats[hit_mat_id].reflection_color;
        }
        */
        // refraction
        if (hit_refraction > 0.0f)
        {
            float offset;
         
            if (dot(r.direction(), hit_normal) > 0) offset = 0.001f;
            if (dot(r.direction(), hit_normal) < 0) offset = -0.001f;

            vec3 H = GGXMicrofacet(hit_normal, mats[hit_mat_id].refraction_roughness).normalize();

            vec3 new_ray_dir = refract(r.direction(), H, hit_ior.x);

            ray new_ray(hit_pos + hit_normal * offset, new_ray_dir);

            int new_depth[] = { depth[0], depth[1], depth[2] - 1 };

            refrac += cast_ray(new_ray, color, mats, g_scene, lights, new_depth, samples, stat) * mats[hit_mat_id].refraction_color;
        }

        float f0 = FresnelReflectionCoef(hit_ior.x, hit_normal, r.direction()) * hit_reflectance;

        kd = (1.0f - f0) * (1.0f - hit_metallic) * (1.0f - hit_refraction);        
        
        //(1.0f - clamp(abs(f0), 0.0f, 1.0f)) * 

        /*
        // indirect lighting
        if (hit_metallic < 1.0f || hit_refraction < 1.0f)
        {
            for (int i = 0; i < samples[2]; i++)
            {
                vec3 rand_ray_dir = random_ray_in_hemisphere(hit_normal);
                ray random_ray(hit_pos + hit_normal * 0.001f, rand_ray_dir);

                color += cast_ray(random_ray, color, mats, g_scene, lights, new_depth, samples, stat) * std::max(0.f, dot(hit_normal, rand_ray_dir)) * new_color * kd;
            }
        }
        */

        float random = generate_random_float();
        vec3 mix(0.0f);
        float mix_reflectance = fit01(hit_reflectance, 0.0f, 0.5f);

        vec3 new_ray_dir(0.0f);
        int new_depth[] = { depth[0], depth[1], depth[2] };

        if (random > mix_reflectance)
        {
            new_ray_dir = random_ray_in_hemisphere(hit_normal);
            new_depth[0] -= 1;
            mix = new_color * kd;
        }
        else
        {
            vec3 H = GGXMicrofacet(hit_normal, hit_roughness).normalize();
            new_ray_dir = reflect(r.direction(), H);
            new_depth[1] -= 1;
            mix = mats[hit_mat_id].reflection_color * clamp(hit_reflectance * f0 + hit_metallic, 0.02f, 1.0f);
        }

        ray new_ray(hit_pos + hit_normal * 0.001f, new_ray_dir);

        indirect += cast_ray(new_ray, color, mats, g_scene, lights, new_depth, samples, stat);

        color += kd * (new_color * inv_pi * radiance) + (refrac * hit_refraction) + ggx * hit_specular_color * hit_specular + indirect * mix;
    }

    // background for dome lights
    else
    {
        for (auto light : lights)
        {
            if (light.type == 3)
            {
                float d = 0.0f;
                color = return_light_int(light, r.direction(), d);
            }
        }
    }

    // nan filtering
    if (std::isnan(color.x) || std::isnan(color.y) || std::isnan(color.z))
    {
        std::cout << color << "\n";
        //color = vec3(0.5f);
    }

    // clamping
    if (color.x > 1.0f || color.y > 1.0f || color.z > 1.0f)
    {
        color = vec3(std::min(color.x, 8.0f), std::min(color.y, 8.0f), std::min(color.z, 8.0f));
    }

    return color;
}


// function used to render a single tile
void batch_render_tile(tile* cur_tile, render_settings& settings, camera& cam, RTCScene g_scene, std::vector<material>& mats, std::vector<light>& lights, stats& stat)
{   
    float scale = tan(deg2rad(cam.fov * 0.5));
    float aspect = (float)settings.xres / (float)settings.yres;

    vec3 up(0, 1, 0);

    vec3 zAxis = ((cam.pos - cam.lookat).normalize());
    vec3 xAxis = (cross(up, zAxis).normalize());
    vec3 yAxis = cross(zAxis, xAxis);

    Matrix44<float> cameraToWorld(xAxis.x, xAxis.y, xAxis.z, 0.0f, yAxis.x, yAxis.y, yAxis.z, 0.0f, zAxis.x, zAxis.y, zAxis.z, 0.0f, cam.pos.x, cam.pos.y, cam.pos.z, 1.0f);

    Vec3f rayOriginWorld, rayPWorld;
    Vec3f campos2(0.0, 0.0, 0.0);

    float pixel[3];

    auto start_tile = std::chrono::system_clock::now();

    for (int y = cur_tile->ystart; y < cur_tile->yend; y++)
    {
        for (int x = cur_tile->xstart; x < cur_tile->xend; x++)
        {
            vec3 col(0.f);

            for (int s = 0; s < settings.samples[0]; s++)
            {
                //AA Box-Muller
                int aa_samples = 1;
                float r1 = generate_random_float();
                float r2 = generate_random_float();
                float dx = sqrt(-0.5 * log(r1)) * cos(2 * M_PI * r2);
                float dy = sqrt(-0.5 * log(r1)) * sin(2 * M_PI * r2);

                float x_ = (2 * (x + 0.5 + dx) / (float)settings.xres - 1) * cam.aspect * scale;
                float y_ = (1 - 2 * (y + 0.5 + dy) / (float)settings.yres) * scale;

                cameraToWorld.multVecMatrix(campos2, rayOriginWorld);
                cameraToWorld.multVecMatrix(Vec3f(x_, y_, -1), rayPWorld);
                Vec3f rayDir = rayPWorld - rayOriginWorld;
                rayDir.normalize();

                vec3 dir(rayDir.x, rayDir.y, rayDir.z);
                ray ray(cam.pos, dir);

                col += (cast_ray(ray, col, mats, g_scene, lights, settings.bounces, settings.samples, stat)) / settings.samples[0];
            }

            col = HableToneMap(col);

            pixel[0] = pow(col.x, 1.0 / 2.2);
            pixel[1] = pow(col.y, 1.0 / 2.2);
            pixel[2] = pow(col.z, 1.0 / 2.2);

            set_tile_pixel(*cur_tile, pixel[0], pixel[1], pixel[2]);
        }
    }

    auto end_tile = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_tile = end_tile - start_tile;
    std::string message = "Tile n°" + std::to_string(cur_tile->id) + " rendered in " + std::to_string(elapsed_tile.count()) + " seconds !";
    if (settings.printer.level == 3) settings.printer.print(message);
}


// function to batch render
void batch_render(render_settings& settings, camera& cam, RTCScene g_scene, std::vector<material>& mats, std::vector<light>& lights, stats& stat)
{
    std::vector<std::future<void>> futures;


    OIIO::ImageSpec spec(settings.xres, settings.yres, 3, OIIO::TypeDesc::FLOAT);
    OIIO::ImageBuf buffer(spec);
    OIIO::ROI roi(0, settings.xres, 0, settings.yres, 0, 3);


    std::vector<tile> tiles = generate_tiles(settings.tile_number, settings.xres, settings.yres, 3);


    std::cout << "Starting batch render..." << std::endl;
    std::cout << "=================================" << std::endl;
    auto start = std::chrono::system_clock::now();
    float progress = 100.0 / (settings.tile_number * settings.tile_number);


    for (auto& tile : tiles)
    {
        futures.push_back(std::async(std::launch::async, batch_render_tile, &tile, settings, cam, g_scene, mats, lights, stat));
    }


    for (int i = 0; i < futures.size(); i++)
    {
        futures[i].wait();
        float p = progress * (i + 1);
        std::cout << "progress : " << std::floor(p) << "%                                        " << "\r";
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

    buffer.write(settings.output_path);
    std::cout << "=================================" << std::endl;
    std::cout << settings.output_path << "\n";
}


// another function to batch render
void batch_render_omp(render_settings& settings, camera& cam, RTCScene g_scene, std::vector<material>& mats, std::vector<light>& lights, stats& stat)
{
    std::cout << "Starting batch render..." << std::endl;
    std::cout << "=================================" << std::endl;
    auto start = std::chrono::system_clock::now();

    float scale = tan(deg2rad(cam.fov * 0.5));
    float aspect = (float)settings.xres / (float)settings.yres;

    vec3 up(0, 1, 0);

    vec3 zAxis = ((cam.pos - cam.lookat).normalize());
    vec3 xAxis = (cross(up, zAxis).normalize());
    vec3 yAxis = cross(zAxis, xAxis);

    Matrix44<float> cameraToWorld(xAxis.x, xAxis.y, xAxis.z, 0.0f, yAxis.x, yAxis.y, yAxis.z, 0.0f, zAxis.x, zAxis.y, zAxis.z, 0.0f, cam.pos.x, cam.pos.y, cam.pos.z, 1.0f);

    Vec3f rayOriginWorld, rayPWorld, randTransformed;
    Vec3f campos2(0.0, 0.0, 0.0);

    float pixel[3];
    float *pixels = new float[settings.xres * settings.yres * 3];

    auto start_tile = std::chrono::system_clock::now();

    float progress = 100.0f / settings.yres;

    float prog = 0;


    for (int y = 0; y < settings.yres; y++)
    {
#pragma omp parallel for schedule(dynamic, 2)
        for (int x = 0; x < settings.xres; x++)
        {
            vec3 col(0.f);

            for (int s = 0; s < settings.samples[0]; s++)
            {
                // AA Box-Muller
                int aa_samples = 1;
                float r1 = generate_random_float();
                float r2 = generate_random_float();
                float dx = sqrt(-0.5 * log(r1)) * cos(2 * M_PI * r2);
                float dy = sqrt(-0.5 * log(r1)) * sin(2 * M_PI * r2);

                Vec3f rand((cam.aperture / 2) * random_in_unit_disk().x * cam.anamorphic_x, (cam.aperture / 2) * random_in_unit_disk().y * cam.anamorphic_y, (cam.aperture / 2) * random_in_unit_disk().z);

                float x_ = (2 * (x + 0.5 + dx) / (float)settings.xres - 1) * cam.aspect * scale;
                float y_ = (1 - 2 * (y + 0.5 + dy) / (float)settings.yres) * scale;

                cameraToWorld.multVecMatrix(campos2, rayOriginWorld);
                cameraToWorld.multVecMatrix(Vec3f(x_, y_, -1), rayPWorld);
                cameraToWorld.multVecMatrix(rand, randTransformed);
                vec3 rayDir = vec3(rayPWorld.x, rayPWorld.y, rayPWorld.z) - vec3(rayOriginWorld.x, rayOriginWorld.y, rayOriginWorld.z);
                
                // depth of field
                vec3 dir = cam.pos + cam.focus_dist * rayDir.normalize();
                vec3 new_pos = cam.pos + vec3(randTransformed.x, randTransformed.y, 0.0f);

                ray ray(new_pos, (dir - new_pos).normalize());

                col += (cast_ray(ray, col, mats, g_scene, lights, settings.bounces, settings.samples, stat)) / settings.samples[0];
            }
            //col = HableToneMap(col);

            col.x = pow(col.x, 1.0 / 2.2);
            col.y = pow(col.y, 1.0 / 2.2);
            col.z = pow(col.z, 1.0 / 2.2);

            pixels[((settings.yres - y - 1) * settings.xres + x) * 3 + 0] = col.x;
            pixels[((settings.yres - y - 1) * settings.xres + x) * 3 + 1] = col.y;
            pixels[((settings.yres - y - 1) * settings.xres + x) * 3 + 2] = col.z;
        }
        prog += progress;

        int p = std::floor(prog);

        if (p % 1 == 0) std::cout << "Progress : " << p << "%                                        " << "\r";
    }

    OIIO::ImageSpec spec(settings.xres, settings.yres, 3, OIIO::TypeDesc::FLOAT);
    OIIO::ImageBuf buffer(spec, pixels);

    buffer = OIIO::ImageBufAlgo::flip(buffer);
    
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Progress : 100%" << "\n";
    std::cout << "=================================" << std::endl;
    std::cout << "Render time : " << elapsed.count() << " seconds" << std::endl;

    buffer.write(settings.output_path);

    std::cout << "=================================" << std::endl;
    std::wcout << "File written to : ";
    std::cout << settings.output_path << "\n";
    std::cout << "=================================" << std::endl;
}


// funtion used to render a single pixel, used for progressive rendering
void render_p(int s, color_t* pixels, int x, int y, render_settings& settings, camera& cam, RTCScene g_scene, std::vector<material>& mats, std::vector<light>& lights, int samples[], int bounces[], stats& stat)
{
    float scale = tan(deg2rad(cam.fov * 0.5));

    vec3 up(0, 1, 0);

    vec3 zAxis = ((cam.pos - cam.lookat).normalize());
    vec3 xAxis = (cross(up, zAxis).normalize());
    vec3 yAxis = cross(zAxis, xAxis);

    Matrix44<float> cameraToWorld(xAxis.x, xAxis.y, xAxis.z, 0.0f, yAxis.x, yAxis.y, yAxis.z, 0.0f, zAxis.x, zAxis.y, zAxis.z, 0.0f, cam.pos.x, cam.pos.y, cam.pos.z, 1.0f);

    Vec3f rayOriginWorld, rayPWorld;
    Vec3f campos2(0.0, 0.0, 0.0);

    auto start_tile = std::chrono::system_clock::now();

    vec3 col(0.f);

    //AA Box-Muller
    int aa_samples = 1;
    float r1 = generate_random_float();
    float r2 = generate_random_float();
    float dx = sqrt(-0.5 * log(r1)) * cos(2 * M_PI * r2);
    float dy = sqrt(-0.5 * log(r1)) * sin(2 * M_PI * r2);

    vec3 rand = (cam.aperture / 2) * (random_in_unit_disk() * vec3(cam.anamorphic_x, cam.anamorphic_y, 1.0f));

    float x_ = (2 * (x + 0.5 + dx) / (float)settings.xres - 1) * cam.aspect * scale;
    float y_ = (1 - 2 * (y + 0.5 + dy) / (float)settings.yres) * scale;

    cameraToWorld.multVecMatrix(campos2, rayOriginWorld);
    cameraToWorld.multVecMatrix(Vec3f(x_, y_, -1), rayPWorld);
    vec3 rayDir = vec3(rayPWorld.x, rayPWorld.y, rayPWorld.z) - vec3(rayOriginWorld.x, rayOriginWorld.y, rayOriginWorld.z);

    // depth of field
    vec3 dir = cam.pos + cam.focus_dist * rayDir.normalize();
    vec3 new_pos = cam.pos + rand;

    ray ray(new_pos, (dir - new_pos).normalize());

    col = cast_ray(ray, col, mats, g_scene, lights, bounces, samples, stat);

    //col = HableToneMap(col);

    if (col.x < 0.0f || col.y < 0.0f || col.z < 0.0f)
    {
        col.x = std::max(col.x, 0.0f);
        col.y = std::max(col.y, 0.0f);
        col.z = std::max(col.z, 0.0f);

    }

    pixels[x + y * settings.xres].R += col.x, 0.45;
    pixels[x + y * settings.xres].G += col.y, 0.45;
    pixels[x + y * settings.xres].B += col.z, 0.45;
}


// function used to render progressively to the screen
void progressive_render(int s, int y, color_t* pixels, render_settings& settings, camera& cam, RTCScene g_scene, std::vector<material>& mats, std::vector<light>& lights, int samples[], int bounces[], stats& stat)
{
    auto start = std::chrono::system_clock::now();

    cam.fov = 2 * rad2deg(std::atan(36.0f / (2 * cam.focal_length)));

#pragma omp parallel for schedule(dynamic, 2)
    for (int z = y; z < std::min(y + 50, settings.yres); z++)
    {
        for (int x = 0; x < settings.xres; x++)
        {
            render_p(s, pixels, x, z, settings, cam, g_scene, mats, lights, samples, bounces, stat);
        }
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::string sample_perf = "Rendered " + std::to_string(1) + " sample in " + std::to_string(elapsed.count()) + " seconds !";
    std::string sample_enum = "Rendered " + std::to_string(s) + " spp";

    //if (settings.printer.level == 3) settings.printer.print(sample_perf);
    //if (settings.printer.level == 3) settings.printer.print(sample_enum);
}


#endif 