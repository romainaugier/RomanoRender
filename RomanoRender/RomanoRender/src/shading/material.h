#pragma once
#include "utils/vec3.h"
#include <OpenImageIO/imagebuf.h>

#ifndef MATERIAL
#define MATERIAL


class material
{
public:
    material() {}

    material(unsigned int &id) :
        mat_id(id)
    {
        //name = "material_" + std::to_string(id);
        clr = Vec3(0.3f);
        roughness = 1.0f;
        refraction = 0.0f;
        islight = false;
        diffuse_roughness = 0.0f;
        specular = 0.0f;
        reflectance = 0.0f;
        metallic = 0.0f;
        sss = 0.0f;
        sss_radius = Vec3(1.0f);
        sss_scale = 1.0f;
        sss_abs = 0.0f;
        sss_steps = 8;
        ior = Vec3(1.3f);
        reflection_color = Vec3(1.0f);
        specular_color = Vec3(1.0f);
        refraction_color = Vec3(1.0f);
        sss_color = Vec3(1.0f);
    }

    material(unsigned int& id, Vec3 color_parm, Vec3 n, bool is_light) :
        clr(color_parm),
        normal(n),
        islight(is_light)
    {        
    }

    material(int& id, std::string _name, Vec3 color_parm, float _roughness, float _refrac) :
        name(_name),
        clr(color_parm),
        roughness(_roughness),
        refraction(_refrac)
    {
        diffuse_roughness = 0.0f;
        specular = 0.0f;
        reflectance = 0.0f;
        metallic = 0.0f;
        sss = 0.0f;
        sss_radius = Vec3(1.0f);
        sss_scale = 1.0f;
        sss_abs = 0.0f;
        sss_steps = 15;
        ior = Vec3(1.0f);
        reflection_color = Vec3(1.0f);
        specular_color = Vec3(1.0f);
        refraction_color = Vec3(1.0f);
        sss_color = Vec3(1.0f);
    }


public:
    unsigned int mat_id;
    std::string name;
    bool islight;
    Vec3 normal;

    Vec3 clr;
    float diffuse_roughness;

    float roughness;
    Vec3 reflection_color;

    float refraction;
    float refraction_roughness;
    Vec3 refraction_color;

    float metallic;
    float reflectance;
    float specular;
    Vec3 specular_color;
    Vec3 ior;

    float sss;
    Vec3 sss_color;
    Vec3 sss_radius;
    float sss_scale;
    int sss_steps;
    float sss_abs;
};


OIIO::ImageBuf load_texture(const char* path)
{
    OIIO::ImageBuf tex(path);
    return tex;
}

Vec3 get_tex_pixel(OIIO::ImageBuf& tex, int x, int y)
{
    float pixel[3];
    tex.OIIO::ImageBuf::getpixel(x, y, 0, pixel);
    return Vec3(pixel[0], pixel[1], pixel[2]);
}

#endif