#include "scene_view.h"


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