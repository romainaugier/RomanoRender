#include "cartoon.h"


vec3 cartoon(int s, std::vector<vec2>& sampler, int samples[], const Ray& r, std::vector<Material> & mats, std::vector<Light*>& lights, int depth[], Render_Settings& settings)
{
	// defining all samples and random numbers we will use
	const float random_float = generate_random_float_fast(s + samples[0]);
	const int sampler_id = (int)(random_float * (sampler.size() - 1));
	const vec2 sample = sampler[sampler_id];

	// ray terminated
	if (depth[0] == 0 || depth[1] == 0 || depth[2] == 0) return vec3(0.0f);
	
	// init embree context
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	RTCRayHit new_ray = r.rayhit;

	vec3 new_color(0.0f);

	rtcIntersect1(settings.scene, &context, &new_ray);

	if (new_ray.hit.geomID != RTC_INVALID_GEOMETRY_ID)
	{
		int hit_mat_id = new_ray.hit.geomID;

		if (mats[hit_mat_id].islight)
		{
			return mats[hit_mat_id].diffuse_color;
		}

		const vec3 hit_normal = vec3(new_ray.hit.Ng_x, new_ray.hit.Ng_y, new_ray.hit.Ng_z).normalize();
		const vec3 hit_pos = vec3(new_ray.ray.org_x, new_ray.ray.org_y, new_ray.ray.org_z) + new_ray.ray.tfar * vec3(new_ray.ray.dir_x, new_ray.ray.dir_y, new_ray.ray.dir_z);
	}
}