#include "maths_utils.h"


// helper functions
vec3 to_polar(const vec2& uv)
{
    float theta = 2.0 * M_PI * uv.x + -M_PI / 2.0;
    float phi = M_PI * uv.y;

    vec3 n;

    n.x = cos(theta) * sin(phi);
    n.y = sin(theta) * sin(phi);
    n.z = cos(phi);

    //n = normalize(n);
    return n;
}


// approx functions
float approx_acos(float x)
{
    float negate = float(x < 0);
    x = abs(x);
    float ret = -0.0187293;
    ret = ret * x;
    ret = ret + 0.0742610;
    ret = ret * x;
    ret = ret - 0.2121144;
    ret = ret * x;
    ret = ret + 1.5707288;
    ret = ret * sqrt(1.0 - x);
    ret = ret - 2 * negate * ret;
    return negate * 3.14159265358979 + ret;
}


float approx_atan(float z)
{
    const float n1 = 0.97239411f;
    const float n2 = -0.19194795f;
    return (n1 + n2 * z * z) * z;
}


float approx_atan2(float y, float x)
{
    const float n1 = 0.97239411f;
    const float n2 = -0.19194795f;
    float result = 0.0f;
    if (x != 0.0f)
    {
        const union { float flVal; int nVal; } tYSign = { y };
        const union { float flVal; int nVal; } tXSign = { x };
        if (fabsf(x) >= fabsf(y))
        {
            union { float flVal; int nVal; } tOffset = { M_PI };
            tOffset.nVal *= tYSign.nVal & 0x80000000u;
            tOffset.nVal *= tXSign.nVal >> 31;
            result = tOffset.flVal;
            const float z = y / x;
            result += (n1 + n2 * z * z) * z;
        }
        else
        {
            union { float flVal; int nVal; } tOffset = { M_PI_2 };
            tOffset.nVal |= tYSign.nVal & 0x80000000u;
            result = tOffset.flVal;
            const float z = x / y;
            result -= (n1 + n2 * z * z) * z;
        }
    }
    else if (y > 0.0f)
    {
        result = M_PI_2;
    }
    else if (y < 0.0f)
    {
        result = -M_PI_2;
    }
    return result;
}


float inv_sqrt(float number)
{
    union {
        float f;
        uint32_t i;
    } conv;

    float x2;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    conv.f = number;
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f = conv.f * (threehalfs - (x2 * conv.f * conv.f));
    return conv.f;
}


// trigonometry functions
void createBasis(const vec3& hit_normal, vec3& tangent, vec3& bitangent)
{
    vec3 up(0.0f, 1.0f, 0.0f);
    if (dot(up, hit_normal) > 0.9f) up = vec3(0.0f, 0.0f, 1.0f);

    tangent = cross(hit_normal, up).normalize();
    bitangent = cross(hit_normal, tangent);
}


void worldToTangent(const vec3& hit_normal, const vec3& tangent, const vec3& bitangent, const vec3& l, const vec3& v, vec3& wo, vec3& wi, vec3& wm)
{
    wo = (v.z * hit_normal + v.x * tangent + v.y * bitangent).normalize();
    wi = (l.z * hit_normal + l.x * tangent + v.y * bitangent).normalize();
    wm = (wo + wi).normalize();
}


vec3 tangentToWorld(const vec3& hit_normal, const vec3& tangent, const vec3& bitangent, const vec3& t)
{
    vec3 wdir;
    wdir = tangent * t.x + hit_normal * t.y + bitangent * t.z;
    return wdir;
}


// schlick/fresnels functions
vec3 schlick_weight(const vec3& f0, const float& h) { return f0 + (1.0f - f0) * std::pow(1 - h, 5); }


float schlick_r0_from_relative_ior(const float eta) { return square(eta - 1.0f) / square(eta + 1.0f); }


float fresnel_reflection_coef(const float n2, const vec3& normal, const vec3& incident)
{
    float n1 = 1.0f;
    float r0 = (n1 - n2) / (n1 + n2);
    float cosX = -dot(normal, incident);

    if (n1 > n2)
    {
        float n = n1 / n2;
        float sint2 = n * n * (1.0f - cosX * cosX);

        if (sint2 > 0.9999f) return 1.0f;
        cosX = sqrt(1.0f - sint2);
    }
    float x = 1.0f - cosX;
    return r0 + (1.0f - r0) * x * x * x * x * x;
}


// pathtracing utils functions
vec3 reflect(const vec3& i, const vec3& n)
{
    return (i - 2 * dot(i, n) * n).normalize();
}


vec3 refract(vec3& i, vec3& n, float ior)
{
    vec3 T(0.f);
    vec3 n_t = n;
    float n1 = 1.0;
    float n2 = ior;
    bool in = true;

    if (dot(i, n) > 0.0)
    {
        n1 = ior;
        n2 = 1.0;
        n_t = -n;
        in = false;
    }

    float dt = dot(i, n_t);
    float n_ior = n1 / n2;

    float radical = 1.0 - (n_ior * n_ior) * (1.0 - dt * dt);

    if (radical > 0.0)
    {
        //T = (n1 / n2) * (i.normalize() - dt * n_t) - n_t * sqrt(radical);
        T = n_ior * i + (n_ior * -dt - sqrt(radical)) * n_t;

        float r0 = ((n1 - n2) / (n1 + n2)) * ((n1 - n2) / (n1 + n2));
        float r;


        if (in) r = r0 + (1 - r0) * std::pow(1 + dot(i, n), 5);
        else r = r0 + (1 - r0) * std::pow(1 - dot(T, n), 5);

        if (generate_random_float_slow() < r) T = reflect(i, n);
        else T = T;
    }
    else T = reflect(i, n);

    return T;
}


vec3 face_forward(const vec3& dir, const vec3& _Ng)
{
    const vec3 Ng = _Ng;
    return dot(dir, Ng) < 0.0f ? Ng : Ng * -1;
}