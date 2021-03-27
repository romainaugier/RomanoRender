#include "sampling_utils.h"


// sampling utils
vec3 sample_ray_in_hemisphere2(const vec3& hit_normal, const vec2& sample)
{
    float a = 1.0f - 2.0f * sample.x;
    float b = sqrtf(1.0f - a * a);
    float phi = 2.0f * M_PI * sample.y;

    return vec3(hit_normal.x + b * cos(phi), hit_normal.y + b * sin(phi), hit_normal.z + a);
}


vec3 sample_ray_in_hemisphere(const vec3& hit_normal, const vec2& sample)
{
    float signZ = (hit_normal.z >= 0.0f) ? 1.0f : -1.0f;
    float a = -1.0f / (signZ + hit_normal.z);
    float b = hit_normal.x * hit_normal.y * a;
    vec3 b1 = vec3(1.0f + signZ * hit_normal.x * hit_normal.x * a, signZ * b, -signZ * hit_normal.x);
    vec3 b2 = vec3(b, signZ + hit_normal.y * hit_normal.y * a, -hit_normal.y);


    float phi = 2.0f * M_PI * sample.x;
    float cosTheta = sqrt(sample.y);
    float sinTheta = sqrt(1.0f - sample.y);
    return ((b1 * cosf(phi) + b2 * sinf(phi)) * cosTheta + hit_normal * sinTheta).normalize();
}


vec3 sample_ray_in_sphere()
{
    return fit01(vec3(generate_random_float_slow(), generate_random_float_slow(), generate_random_float_slow()), vec3(-1.0f), vec3(1.0f));
}


vec3 sample_unit_disk(int state)
{
    vec3 p;
    do {
        p = 2.0 * vec3(generate_random_float_fast(state), generate_random_float_fast(state + 1), 0) - vec3(1.0f, 1.0f, 0.0f);
    } while (dot(p, p) >= 1);
    return p;
}


// random generator utils
int wang_hash(int seed)
{
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9u;
    seed = seed ^ (seed >> 4u);
    seed *= 0x27d4eb2du;
    seed = seed ^ (seed >> 15u);
    return 1u + seed;
}


int xorshift32(int state)
{
    int x = state;
    x ^= x << 13u;
    x ^= x >> 17u;
    x ^= x << 5u;
    return x;
}


float generate_random_float_slow()
{
    unsigned int tofloat = 0x2f800004u;
    static unsigned long x = 123456789, y = 362436069, z = 521288629;

    unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    float a = static_cast<float>(z) * reinterpret_cast<const float&>(tofloat);

    return a;
}


float generate_random_float_fast(int state)
{
    unsigned int tofloat = 0x2f800004u;
    state = wang_hash(state);
    int x = xorshift32(state);
    state = x;
    return static_cast<float>(x) * reinterpret_cast<const float&>(tofloat) + 0.5f;
}


float exponential_distribution(float& sigma)
{
    float xi = generate_random_float_slow() - 0.001f;
    return -logf(1.0f - xi) / sigma;
}


// sample sequence loading utils
std::vector<vec2> load_sample_sequence(const char* file)
{
    std::string line;
    std::fstream seq(file);

    if (seq.is_open())
    {
        std::vector<vec2> sequence;

        while (getline(seq, line))
        {
            double a, b;
            sscanf(line.c_str(), "(%lf, %lf),", &a, &b);

            vec2 sample(a, b);
            sequence.push_back(sample);
        }

        seq.close();

        return sequence;
    }
}


std::vector<std::vector<vec2>> load_sequences(const char* directory)
{
    boost::filesystem::path full_path = boost::filesystem::system_complete("../Samples");

    // full_path.string().c_str();

    std::vector<std::vector<vec2>> sequences;

    for (boost::filesystem::directory_entry& entry : boost::filesystem::directory_iterator(directory))
    {
        std::string to_s = entry.path().string();
        std::vector<vec2> seq = load_sample_sequence(to_s.c_str());
        sequences.push_back(seq);
    }

    return sequences;
}