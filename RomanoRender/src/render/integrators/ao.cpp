#include "ao.h"


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