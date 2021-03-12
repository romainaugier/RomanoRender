#include "integrators.h"


// ray cast function used to render. Return a vec3 color
vec3 pathtrace(int s, std::vector<vec2>& sampler, const Ray& r, vec3 color, std::vector<Material>& mats, Render_Settings& settings, std::vector<Light*>& lights, int depth[], std::vector<int>& light_path, int samples[], Stats& stat)
{

    //const float random_float = generate_random_float();
    __int64 seed = s + samples[0];
    const float random_float = generate_random_float_fast(seed);
    const int sampler_id = (int)(random_float * (sampler.size() - 1));
    const vec2 sample = sampler[sampler_id];

    stat.add_ray();

    if (depth[0] == 0 || depth[1] == 0 || depth[2] == 0 || depth[3] == 0) return vec3(0.0f);

    RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    float near_clipping = 0.0f;
    float far_clipping = 10000.0f;

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

    rtcIntersect1(settings.scene, &context, &rayhit);

    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
    {
        int hit_mat_id = rayhit.hit.geomID;

        if (mats[hit_mat_id].islight && dot(mats[hit_mat_id].normal, r.direction()) < 0 && depth[0] == 6)
        {
            return mats[hit_mat_id].clr;
        }

        new_color = mats[hit_mat_id].clr;

        float hit_diff_roughness = mats[hit_mat_id].diffuse_roughness;
        float hit_roughness = std::max(0.005f, mats[hit_mat_id].roughness);
        float hit_refraction = mats[hit_mat_id].refraction;
        float hit_metallic = mats[hit_mat_id].metallic;
        float hit_specular = mats[hit_mat_id].specular;
        float hit_reflectance = mats[hit_mat_id].reflectance;
        float hit_sss = mats[hit_mat_id].sss;

        vec3 hit_specular_color = mats[hit_mat_id].specular_color;
        vec3 hit_ior = mats[hit_mat_id].ior;
        vec3 hit_refl_color = mats[hit_mat_id].reflection_color;
        vec3 hit_sss_color = mats[hit_mat_id].sss_color;

        vec3 hit_normal = vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z).normalize();
        vec3 hit_pos = vec3(rayhit.ray.org_x, rayhit.ray.org_y, rayhit.ray.org_z) + rayhit.ray.tfar * vec3(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z);

        vec3 kd(1.0f);
        vec3 ks(0.5f);
        vec3 radiance(0.0f);
        vec3 specular(0.0f);
        vec3 refrac(0.0f);
        vec3 trans(0.0f);
        vec3 indirect(0.0f);
        vec3 ggx(0.0f);


        for (auto light : lights)
        {
            for (int i = 0; i < samples[1]; i++)
            {
                // used to convert parent ptr to subtype ptr to get subtype specific members using branched dynamic cast
                Point_Light* ptlight = nullptr;
                Distant_Light* distlight = nullptr;
                Square_Light* sqlight = nullptr;
                Dome_Light* domelight = nullptr;

                vec3 area_sample_position(0.0f);
                int hdri_id = 1;

                vec3 ray_dir = light->return_ray_direction(hit_pos, sample);

                Ray new_ray(hit_pos, ray_dir);

                float distance = 10000.0f;
                float area_shadow = 1.0f;

                if (ptlight = dynamic_cast<Point_Light*>(light)) distance = dist(hit_pos, ptlight->position) - 0.001f;

                else if (sqlight = dynamic_cast<Square_Light*>(light))
                {
                    if (dot(ray_dir, sqlight->normal) > 0) continue;
                    else
                    {
                        vec3 pos = vec3(sqlight->transform_mat[4], sqlight->transform_mat[8], sqlight->transform_mat[12]);
                        distance = dist(hit_pos, area_sample_position) - 0.001f;
                        float d = dot(sqlight->normal, ray_dir);
                        area_shadow = -d;
                    }
                }

                float NdotL = std::max(0.f, dot(hit_normal, ray_dir));

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


                rtcOccluded1(settings.scene, &context, &shadow);

                if (shadow.tfar > 0.0f)
                {
                    radiance += (light->return_light_throughput(distance) * NdotL * area_shadow);

                    if (hit_specular > 0.0f)
                    {
                        vec3 H = (ray_dir + -r.direction()).normalize();
                        float NdotH = Saturate(dot(hit_normal, H));
                        float LdotH = Saturate(dot(ray_dir, H));
                        float NdotV = Saturate(dot(hit_normal, -r.direction()));

                        float D = ggx_normal_distribution(NdotH, hit_roughness);
                        float G = schlick_masking_term(NdotL, NdotV, hit_roughness);
                        vec3 F = schlick_fresnel(hit_ior, LdotH);

                        ggx += (D * G * F / (4 * std::max(0.001f, NdotV)));
                        
                    }
                }
            }
        }

        vec3 mix(1.0f);
        const float f0 = fresnel_reflection_coef(hit_ior.x, hit_normal, r.direction()) * hit_reflectance;
        kd = (1.0f - clamp(abs(f0), 0.02f, 1.0f)) * (1.0f - hit_metallic) * (1.0f - hit_refraction);

        // refraction
        if (hit_refraction > 0.0f)
        {
            float offset;

            if (dot(r.direction(), hit_normal) > 0) offset = 0.001f;
            if (dot(r.direction(), hit_normal) < 0) offset = -0.001f;

            vec3 H = ggx_microfacet(hit_normal, mats[hit_mat_id].refraction_roughness, sample);

            vec3 new_ray_dir = refract(r.direction(), H.normalize(), hit_ior.x);

            Ray new_ray(hit_pos + hit_normal * offset, new_ray_dir);

            int new_depth[] = { depth[0], depth[1], depth[2] - 1, depth[3] };

            light_path.push_back(3);

            refrac += pathtrace(s, sampler, new_ray, color, mats, settings, lights, new_depth, light_path, samples, stat) * mats[hit_mat_id].refraction_color;
        }

        if (hit_sss > 0.0f)
        {
            int new_depth[] = { depth[0], depth[1], depth[2], depth[3] - 1 };

            vec3 radius = mats[hit_mat_id].sss_radius;
            float scale = mats[hit_mat_id].sss_scale;
            float absorption = 1 - mats[hit_mat_id].sss_abs;
            int steps = mats[hit_mat_id].sss_steps;
            bool transmitted = false;
            float step = 0.0f;

            float pdf = 1.0f;

            vec3 position = hit_pos + hit_normal * -0.001f;
            vec3 direction = sample_ray_in_hemisphere(-hit_normal, sample);

            float step_size = scale;

            vec3 start_position = position;
            vec3 end_position(0.0f);
            vec3 end_normal(0.0f);


            for (int i = 1; i < steps + 1; i++)
            {
                //if (generate_random_float() > 0.3f) break;

                RTCRayHit shadow;
                shadow.ray.org_x = position.x;
                shadow.ray.org_y = position.y;
                shadow.ray.org_z = position.z;
                shadow.ray.dir_x = direction.x;
                shadow.ray.dir_y = direction.y;
                shadow.ray.dir_z = direction.z;
                shadow.ray.tnear = 0.0f;
                shadow.ray.tfar = step_size;
                shadow.ray.mask = -1;
                shadow.ray.flags = 0;
                shadow.hit.geomID = RTC_INVALID_GEOMETRY_ID;
                shadow.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

                rtcIntersect1(settings.scene, &context, &shadow);

                if (shadow.hit.geomID != RTC_INVALID_GEOMETRY_ID)
                {
                    transmitted = true;
                    step = shadow.ray.tfar;
                    end_position = position + direction * step;
                    end_normal = vec3(shadow.hit.Ng_x, shadow.hit.Ng_y, shadow.hit.Ng_z);

                    break;
                }

                float random = generate_random_float_fast(seed * i);
                step_size = (random * 2) * scale / i;

                position += direction * step_size;

                direction = sample_ray_in_hemisphere(direction, sample);
            }

            float walked_distance = dist(start_position, end_position);
            float t = 1 - exp(-walked_distance);

            if (transmitted)
            {
                vec3 sss_light_contribution(0.0f);

                //float a = 1.0f / powf((1.0f + scale * walked_distance / bleed), bleed);

                for (auto light : lights)
                {
                    for (int i = 0; i < samples[1]; i++)
                    {
                        // used to convert parent ptr to subtype ptr to get subtype specific members using branched dynamic cast
                        Point_Light* ptlight = nullptr;
                        Distant_Light* distlight = nullptr;
                        Square_Light* sqlight = nullptr;
                        Dome_Light* domelight = nullptr;

                        vec3 area_sample_position(0.0f);
                        int hdri_id = 1;

                        vec3 ray_dir = light->return_ray_direction(hit_pos, sample);

                        Ray new_ray(hit_pos, ray_dir);

                        float distance = 10000.0f;
                        float area_shadow = 1.0f;

                        if (ptlight = dynamic_cast<Point_Light*>(light)) distance = dist(hit_pos, ptlight->position) - 0.001f;

                        else if (sqlight = dynamic_cast<Square_Light*>(light))
                        {
                            if (dot(ray_dir, sqlight->normal) > 0) continue;
                            else
                            {
                                vec3 pos = vec3(sqlight->transform_mat[4], sqlight->transform_mat[8], sqlight->transform_mat[12]);
                                distance = dist(hit_pos, area_sample_position) - 0.001f;
                                float d = dot(sqlight->normal, ray_dir);
                                area_shadow = -d;
                            }
                        }

                        float NdotL = std::max(0.f, dot(hit_normal, ray_dir));

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


                        rtcOccluded1(settings.scene, &context, &shadow);

                        if (shadow.tfar > 0.0f)
                        {
                            sss_light_contribution += (light->return_light_throughput(distance) * NdotL * area_shadow * inv_pi);
                        }
                    }
                }

                vec3 transmitted_color = vec3(hit_sss_color.x * fit(t * absorption, 0.0f, radius.x, 1.0f, 0.0f),
                    hit_sss_color.y * fit(t * absorption, 0.0f, radius.y, 1.0f, 0.0f),
                    hit_sss_color.z * fit(t * absorption, 0.0f, radius.z, 1.0f, 0.0f));

                trans += sss_light_contribution * transmitted_color;
            }
        }

        // diffuse / reflection
        if (hit_refraction < 1.0f)
        {
            float random = 1.0f;
            float mix_reflectance = 0.0f;

            if (hit_reflectance > 0.0f)
            {
                random = random_float;
                mix_reflectance = fit01(hit_reflectance, 0.0f, 0.5f);
            }

            vec3 new_ray_dir(0.0f);
            int new_depth[] = { depth[0], depth[1], depth[2] };

            if (random > mix_reflectance)
            {
                new_ray_dir = sample_ray_in_hemisphere(hit_normal, sample);
                light_path.push_back(1);
                new_depth[0] -= 1;
                mix = new_color * kd;
            }
            else
            {
                vec3 H = ggx_microfacet(hit_normal, hit_roughness, sample);
                new_ray_dir = reflect(r.direction(), H.normalize());
                light_path.push_back(2);
                new_depth[1] -= 1;
                mix = (hit_reflectance * clamp(f0, 0.04f, 1.0f) + hit_metallic) * mats[hit_mat_id].reflection_color;
            }

            Ray new_ray(hit_pos + hit_normal * 0.001f, new_ray_dir);

            indirect += pathtrace(s, sampler, new_ray, color, mats, settings, lights, new_depth, light_path, samples, stat);
        }



        color += kd * (new_color * inv_pi * radiance) + (refrac * hit_refraction) + ggx * hit_specular_color * hit_specular + indirect * mix + trans * hit_sss;
    }


    // background for dome lights
    else
    {
        for (auto light : lights)
        {
            Dome_Light* domelight = nullptr;

            if (domelight = dynamic_cast<Dome_Light*>(light))
            {
                int id = 0;

                if (depth[0] >= 16 || depth[0] >= 16 && depth[1] < 6 || depth[0] >= 16 && depth[2] < 10) id = -1;
                if (light_path.size() > 1 && light_path[0] == 1 && light_path[1] == 3) id = -1;

                if (depth[0] >= 16 && !domelight->visible) return vec3(0.0f);

                float d = 0.0f;
                color = domelight->return_light_throughput(d);
            }
        }
    }


    // nan filtering
    if (std::isnan(color.x) || std::isnan(color.y) || std::isnan(color.z))
    {
        //std::cout << color << "\n";
        color = vec3(0.5f);
    }

    // clamping
    if (color.x > 1.0f || color.y > 1.0f || color.z > 1.0f)
    {
        color = vec3(std::min(color.x, 8.0f), std::min(color.y, 8.0f), std::min(color.z, 8.0f));
    }

    return color;
}


// funtion used to render a single pixel, used for progressive rendering
void render_p(int s, std::vector<vec2>& sampler, color_t* pixels, int x, int y, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat)
{
    float scale = tan(deg2rad(cam.fov * 0.5));

    vec3 up(0, 1, 0);

    vec3 zAxis = ((cam.pos - cam.lookat).normalize());
    vec3 xAxis = (cross(up, zAxis).normalize());
    vec3 yAxis = cross(zAxis, xAxis);

    mat44 cameraToWorld(xAxis.x, xAxis.y, xAxis.z, 0.0f, yAxis.x, yAxis.y, yAxis.z, 0.0f, zAxis.x, zAxis.y, zAxis.z, 0.0f, cam.pos.x, cam.pos.y, cam.pos.z, 1.0f);

    vec3 campos2(0.0, 0.0, 0.0);

    auto start_tile = std::chrono::system_clock::now();

    vec3 col(0.f);

    //AA Box-Muller
    vec2 d = sampler[s];

    float dx, dy;

    dx = sqrt(-0.5 * log(d.x)) * cos(2 * M_PI * d.y);
    dy = sqrt(-0.5 * log(d.x)) * sin(2 * M_PI * d.y);

    vec3 rand(0.0f);

    if (cam.aperture > 0.0f) rand = (cam.aperture / 2) * sample_unit_disk();// *vec3(cam.anamorphic_x, cam.anamorphic_y, 1.0f));

    float x_ = (2 * (x + dx) / (float)settings.xres - 1) * cam.aspect * scale;
    float y_ = (1 - 2 * (y + dy) / (float)settings.yres) * scale;

    vec3 sample_pos = vec3(x_, y_, -1);

    //cameraToWorld.multVecMatrix(campos2, rayOriginWorld);
    //cameraToWorld.multVecMatrix(vec3(x_, y_, -1), rayPWorld);

    vec3 rayOriginWorld = transform(campos2, cameraToWorld);
    vec3 rayPWorld = transform(sample_pos, cameraToWorld);
    vec3 rayDir = vec3(rayPWorld.x, rayPWorld.y, rayPWorld.z) - vec3(rayOriginWorld.x, rayOriginWorld.y, rayOriginWorld.z);

    // depth of field
    vec3 dir = cam.pos + cam.focus_dist * rayDir.normalize();
    vec3 new_pos = cam.pos + rand;

    Ray ray(new_pos, (dir - new_pos).normalize());

    std::vector<int> light_path;

    col = pathtrace(s * x * y, sampler, ray, col, mats, settings, lights, bounces, light_path, samples, stat);

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
    float scale = tan(deg2rad(cam.fov * 0.5));

    vec3 up(0, 1, 0);

    vec3 zAxis = ((cam.pos - cam.lookat).normalize());
    vec3 xAxis = (cross(up, zAxis).normalize());
    vec3 yAxis = cross(zAxis, xAxis);

    mat44 cameraToWorld(xAxis.x, xAxis.y, xAxis.z, 0.0f, yAxis.x, yAxis.y, yAxis.z, 0.0f, zAxis.x, zAxis.y, zAxis.z, 0.0f, cam.pos.x, cam.pos.y, cam.pos.z, 1.0f);

    vec3 campos2(0.0, 0.0, 0.0);

    auto start_tile = std::chrono::system_clock::now();

    vec3 col(0.f);

    //AA Box-Muller
    vec2 d = sampler[s];
    float dx = sqrt(-0.5 * log(d.x)) * cos(2 * M_PI * d.y);
    float dy = sqrt(-0.5 * log(d.x)) * sin(2 * M_PI * d.y);

    vec3 rand = (cam.aperture / 2) * sample_unit_disk();// *vec3(cam.anamorphic_x, cam.anamorphic_y, 1.0f));

    float x_ = (2 * (x + dx) / (float)settings.xres - 1) * cam.aspect * scale;
    float y_ = (1 - 2 * (y + dy) / (float)settings.yres) * scale;

    vec3 sample_pos = vec3(x_, y_, -1);

    //cameraToWorld.multVecMatrix(campos2, rayOriginWorld);
    //cameraToWorld.multVecMatrix(vec3(x_, y_, -1), rayPWorld);

    vec3 rayOriginWorld = transform(campos2, cameraToWorld);
    vec3 rayPWorld = transform(sample_pos, cameraToWorld);
    vec3 rayDir = vec3(rayPWorld.x, rayPWorld.y, rayPWorld.z) - vec3(rayOriginWorld.x, rayOriginWorld.y, rayOriginWorld.z);

    // depth of field
    vec3 dir = cam.pos + cam.focus_dist * rayDir.normalize();
    vec3 new_pos = cam.pos + rand;

    Ray ray(new_pos, (dir - new_pos).normalize());

    std::vector<int> light_path;

    if (generate_random_float_fast(x * y + s) > 0.75f)
        col = pathtrace(s, sampler, ray, col, mats, settings, lights, bounces, light_path, samples, stat);

    //col = HableToneMap(col);

    if (col.x < 0.0f || col.y < 0.0f || col.z < 0.0f)
    {
        col.x = std::max(col.x, 0.0f);
        col.y = std::max(col.y, 0.0f);
        col.z = std::max(col.z, 0.0f);

    }

    pixels[x + y * (settings.xres)].R += col.x, 0.45;
    pixels[x + y * (settings.xres)].G += col.y, 0.45;
    pixels[x + y * (settings.xres)].B += col.z, 0.45;
}


// function used to render progressively to the screen
void progressive_render(int s, int* ids, int y, std::vector<std::vector<vec2>>& sampler, color_t* pixels, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat)
{
    auto start = std::chrono::system_clock::now();

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
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::string sample_perf = "Rendered " + std::to_string(1) + " sample in " + std::to_string(elapsed.count()) + " seconds !";
    std::string sample_enum = "Rendered " + std::to_string(s) + " spp";

    //if (settings.printer.level == 3) settings.printer.print(sample_perf);
    //if (settings.printer.level == 3) settings.printer.print(sample_enum);
}


void progressive_render_fast(int s, int* ids, std::vector<std::vector<vec2>>& sampler, color_t* pixels, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat)
{
    auto start = std::chrono::system_clock::now();

    cam.fov = 2 * rad2deg(std::atan(36.0f / (2 * cam.focal_length)));

#pragma omp parallel for
    for (int z = 0; z < settings.yres; z++)
    {
        for (int x = 0; x < settings.xres; x++)
        {
            if (generate_random_float_fast(x * z + 1) > 0.75f)
            {
                int id = ids[x + z * settings.xres];
                render_p(s, sampler[id], pixels, x, z, settings, cam, mats, lights, samples, bounces, stat);
            }
        }
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::string sample_perf = "Rendered " + std::to_string(1) + " sample in " + std::to_string(elapsed.count()) + " seconds !";
    std::string sample_enum = "Rendered " + std::to_string(s) + " spp";

    //if (settings.printer.level == 3) settings.printer.print(sample_perf);
    //if (settings.printer.level == 3) settings.printer.print(sample_enum);
}

