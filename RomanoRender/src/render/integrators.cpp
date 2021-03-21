#include "integrators.h"


// pathtracing integrator. Return a vec3 color
vec3 pathtrace(int s, std::vector<vec2>& sampler, const Ray& r, vec3 color, std::vector<Material>& mats, Render_Settings& settings, std::vector<Light*>& lights, int depth[], std::vector<int>& light_path, int samples[], Stats& stat)
{
    // defining all the randoms and pseudo randoms we'll need for the samples
    const float random_float = generate_random_float_fast(s + samples[0]);
    const int sampler_id = (int)(random_float * (sampler.size() - 1));
    const vec2 sample = sampler[sampler_id];

    stat.add_ray();

    // ray terminated
    if (depth[0] == 0 || depth[1] == 0 || depth[2] == 0 || depth[3] == 0) return vec3(0.0f);

    // initialize embree context
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    float near_clipping = 0.0f;
    float far_clipping = 10000.0f;

    RTCRayHit new_ray = r.rayhit;

    vec3 new_color(0.0f);

    rtcIntersect1(settings.scene, &context, &new_ray);

    if (new_ray.hit.geomID != RTC_INVALID_GEOMETRY_ID)
    {
        int hit_mat_id = new_ray.hit.geomID;

        new_color = mats[hit_mat_id].diffuse_color;

        const float hit_diff_roughness = mats[hit_mat_id].diffuse_roughness;
        const float hit_roughness = std::max(0.005f, mats[hit_mat_id].roughness);
        const float hit_refraction = mats[hit_mat_id].refraction;
        const float hit_metallic = mats[hit_mat_id].metallic;
        const float hit_specular = mats[hit_mat_id].specular;
        const float hit_reflectance = mats[hit_mat_id].reflectance;
        const float hit_sss = mats[hit_mat_id].sss;

        const vec3 hit_specular_color = mats[hit_mat_id].specular_color;
        const vec3 hit_ior = mats[hit_mat_id].ior;
        const vec3 hit_refl_color = mats[hit_mat_id].reflection_color;
        const vec3 hit_sss_color = mats[hit_mat_id].sss_color;

        const vec3 hit_normal = vec3(new_ray.hit.Ng_x, new_ray.hit.Ng_y, new_ray.hit.Ng_z).normalize();
        const vec3 hit_pos = vec3(new_ray.ray.org_x, new_ray.ray.org_y, new_ray.ray.org_z) + new_ray.ray.tfar * vec3(new_ray.ray.dir_x, new_ray.ray.dir_y, new_ray.ray.dir_z);

        vec3 kd(1.0f);
        vec3 ks(0.5f);
        vec3 radiance(0.0f);
        vec3 specular(0.0f);
        vec3 refrac(0.0f);
        vec3 trans(0.0f);
        vec3 indirect(0.0f);
        vec3 ggx(0.0f);

        // direct Lighting
        for (auto light : lights)
        {
            for (int i = 0; i < samples[1]; i++)
            {
                // use to convert parent ptr to subtype ptr to get subtype specific members using branched dynamic cast
                Point_Light* ptlight = nullptr;
                Distant_Light* distlight = nullptr;
                Square_Light* sqlight = nullptr;
                Dome_Light* domelight = nullptr;

                float distance = 10000.0f;
                float area_shadow = 1.0f;
                int hdri_id = 1;

                vec3 ray_dir = light->return_ray_direction(hit_pos, sample);

                // point light
                if (ptlight = dynamic_cast<Point_Light*>(light))
                {
                    ray_dir = ptlight->return_ray_direction(hit_pos, sample);
                    distance = dist(hit_pos, ptlight->position) - 0.001f;
                }

                // distant light
                else if (distlight = dynamic_cast<Distant_Light*>(light)) ray_dir = distlight->return_ray_direction(hit_pos, sample);

                // dome light
                else if (domelight = dynamic_cast<Dome_Light*>(light)) ray_dir = domelight->return_ray_direction(hit_pos, sample);

                // square light
                if (sqlight = dynamic_cast<Square_Light*>(light))
                {
                    const vec3 light_sample_pos = sqlight->return_ray_direction(hit_pos, sample);
                    ray_dir = (light_sample_pos - hit_pos).normalize();

                    if (dot(ray_dir, sqlight->normal) > 0) continue;
                    else
                    {
                        distance = dist(hit_pos, light_sample_pos) - 0.001f;
                        float d = dot(sqlight->normal, ray_dir);
                        area_shadow = -d;
                    }
                }

                Ray shadow(hit_pos, ray_dir, 0.001f, distance);

                const float NdotL = std::max(0.f, dot(hit_normal, ray_dir));

                rtcOccluded1(settings.scene, &context, &shadow.ray);

                if (shadow.ray.tfar > 0.0f)
                {
                    // point light
                    if (ptlight = dynamic_cast<Point_Light*>(light)) radiance += ptlight->return_light_throughput(distance) * NdotL * area_shadow;

                    // distant light
                    else if (distlight = dynamic_cast<Distant_Light*>(light)) radiance += distlight->return_light_throughput(distance) * NdotL * area_shadow;

                    // dome light
                    else if (domelight = dynamic_cast<Dome_Light*>(light)) radiance += domelight->return_light_throughput(distance) * NdotL * area_shadow;

                    // square light
                    if (sqlight = dynamic_cast<Square_Light*>(light)) radiance += sqlight->return_light_throughput(distance) * NdotL * area_shadow;

                    if (hit_reflectance > 0.0f)
                    {
                        const vec3 H = (ray_dir + -r.direction).normalize();
                        const float NdotH = Saturate(dot(hit_normal, H));
                        const float LdotH = Saturate(dot(ray_dir, H));
                        const float NdotV = Saturate(dot(hit_normal, -r.direction));

                        const float D = ggx_normal_distribution(NdotH, hit_roughness);
                        const float G = schlick_masking_term(NdotL, NdotV, hit_roughness);
                        const vec3 F = schlick_fresnel(hit_ior, LdotH);

                        ggx += (D * G * F / (4 * std::max(0.001f, NdotV)));
                        
                    }
                }
            }
        }

        vec3 mix(1.0f);
        const float f0 = fresnel_reflection_coef(hit_ior.x, hit_normal, r.direction) * hit_reflectance;
        kd = (1.0f - clamp(abs(f0), 0.02f, 1.0f)) * (1.0f - hit_metallic) * (1.0f - hit_refraction);

        // refraction
        if (hit_refraction > 0.0f)
        {
            float offset;

            if (dot(r.direction, hit_normal) > 0) offset = 0.001f;
            if (dot(r.direction, hit_normal) < 0) offset = -0.001f;

            vec3 H = ggx_microfacet(hit_normal, mats[hit_mat_id].refraction_roughness, sample);

            vec3 new_ray_dir = refract(r.direction, H.normalize(), hit_ior.x);

            Ray new_ray(hit_pos + hit_normal * offset, new_ray_dir);

            int new_depth[] = { depth[0], depth[1], depth[2] - 1, depth[3] };

            light_path.push_back(3);

            refrac += pathtrace(s, sampler, new_ray, color, mats, settings, lights, new_depth, light_path, samples, stat) * mats[hit_mat_id].refraction_color;
        }

        // subsurface scattering
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

                Ray ray(position, direction, 0.0f, step_size);

                rtcIntersect1(settings.scene, &context, &ray.rayhit);

                if (ray.rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
                {
                    transmitted = true;
                    step = ray.rayhit.ray.tfar;
                    end_position = position + direction * step;
                    end_normal = vec3(ray.rayhit.hit.Ng_x, ray.rayhit.hit.Ng_y, ray.rayhit.hit.Ng_z);

                    break;
                }

                float random = generate_random_float_fast(s + samples[0]);
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

                        if (sqlight = dynamic_cast<Square_Light*>(light))
                        {
                            const vec3 light_sample_pos = sqlight->return_ray_direction(hit_pos, sample);
                            ray_dir = (light_sample_pos - hit_pos).normalize();

                            if (dot(ray_dir, sqlight->normal) > 0) continue;
                            else
                            {
                                distance = dist(hit_pos, light_sample_pos) - 0.001f;
                                float d = dot(sqlight->normal, ray_dir);
                                area_shadow = -d;
                            }
                        }

                        float NdotL = std::max(0.f, dot(hit_normal, ray_dir));

                        Ray shadow(new_ray.origin, new_ray.direction, 0.001f, distance);

                        rtcOccluded1(settings.scene, &context, &shadow.ray);

                        if (shadow.ray.tfar > 0.0f)
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
                new_ray_dir = reflect(r.direction, H.normalize());
                light_path.push_back(2);
                new_depth[1] -= 1;
                mix = (hit_reflectance * clamp(f0, 0.04f, 1.0f) + hit_metallic) * mats[hit_mat_id].reflection_color;
            }

            Ray new_ray(hit_pos + hit_normal * 0.001f, new_ray_dir);

            indirect += pathtrace(s, sampler, new_ray, color, mats, settings, lights, new_depth, light_path, samples, stat);
        }



        color += kd * (new_color * inv_pi * radiance) + (refrac * hit_refraction) + ggx * hit_reflectance + indirect * mix + trans * hit_sss;
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

                //if (depth[0] >= 6 || depth[0] >= 6 && depth[1] < 6 || depth[0] >= 6 && depth[2] < 10) id = -1;
                
                //else if (light_path.size() > 1 && light_path[0] == 1 && light_path[1] == 3) id = -1;

                if (depth[0] >= 6 && !domelight->visible) return vec3(0.0f);

                const float d = 0.0f;
                return domelight->return_light_throughput(d);
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


// ao integrator
vec3 ambient_occlusion(int s, std::vector<vec2>& sampler, const Ray& r, Render_Settings& settings)
{
    // defining all the randoms and pseudo randoms we'll need for the samples
    const float random_float = generate_random_float_fast(s);
    const int sampler_id = (int)(random_float * (sampler.size() - 1));
    const vec2 sample = sampler[sampler_id];

    // initialize embree context
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    float near_clipping = 0.0f;
    float far_clipping = 10000.0f;

    RTCRayHit new_ray = r.rayhit;

    rtcIntersect1(settings.scene, &context, &new_ray);

    if (new_ray.hit.geomID != RTC_INVALID_GEOMETRY_ID)
    {
        const vec3 hit_normal = vec3(new_ray.hit.Ng_x, new_ray.hit.Ng_y, new_ray.hit.Ng_z).normalize();
        const vec3 hit_pos = vec3(new_ray.ray.org_x, new_ray.ray.org_y, new_ray.ray.org_z) + new_ray.ray.tfar * vec3(new_ray.ray.dir_x, new_ray.ray.dir_y, new_ray.ray.dir_z);

        const vec3 new_ray_dir = sample_ray_in_hemisphere(hit_normal, sample);

        Ray shadow(hit_pos + hit_normal * 0.001f, new_ray_dir, 0.0f, 3.0f);

        rtcOccluded1(settings.scene, &context, &shadow.ray);

        if (shadow.ray.tfar > 0.0f)
        {
            return vec3(fit(shadow.ray.tfar, 0.0f, 3.0f, 0.0f, 1.0f));
        }

        else return vec3(0.0f);
    }

    return vec3(0.0f);
}


// simple pt scene viewer
vec3 scene_viewer(const Ray& r, Render_Settings& settings)
{
    // initialize embree context
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    float near_clipping = 0.0f;
    float far_clipping = 10000.0f;

    RTCRayHit new_ray = r.rayhit;

    rtcIntersect1(settings.scene, &context, &new_ray);

    if (new_ray.hit.geomID != RTC_INVALID_GEOMETRY_ID)
    {
        const vec3 hit_normal = vec3(new_ray.hit.Ng_x, new_ray.hit.Ng_y, new_ray.hit.Ng_z).normalize();
        
        return vec3(fit(dot(hit_normal, r.direction), -1.0f, 0.0f, 0.5f, 0.25f));
    }

    return vec3(0.0f);
}


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
    vec3 rand = (cam.aperture / 2.0f) * sample_unit_disk();
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
    else if (settings.integrator == 1) col = ambient_occlusion(s * x * y, sampler, ray, settings);
    else if (settings.integrator == 2) col = scene_viewer(ray, settings);

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
    vec3 rand = (cam.aperture / 2.0f) * sample_unit_disk();
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
    else if (settings.integrator == 1) col = ambient_occlusion(s * x * y, sampler, ray, settings);
    else if (settings.integrator == 2) col = scene_viewer(ray, settings);

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


void progressive_render_fast(int s, int* ids, std::vector<std::vector<vec2>>& sampler, color_t* pixels, Render_Settings& settings, Camera& cam, std::vector<Material>& mats, std::vector<Light*>& lights, int samples[], int bounces[], Stats& stat)
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

