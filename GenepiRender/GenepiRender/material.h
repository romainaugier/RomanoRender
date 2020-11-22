#pragma once
#include "vec3.h"
#include <OpenImageIO/imagebuf.h>

#ifndef MATERIAL
#define MATERIAL


class material
{
public:
    material() {}

    material(int &id) :
        mat_id(id)
    {
    }

    material(int& id, vec3 color_parm, vec3 n, bool is_light) :
        clr(color_parm),
        normal(n),
        islight(is_light)
    {        
    }

    material(int& id, vec3 color_parm, float _roughness, float _refrac) :
        clr(color_parm),
        roughness(_roughness),
        refraction(_refrac)
    {
    }

    material(int& id, vec3 color_parm, float roughness_parm, float reflectance_parm, float metallic_parm, float sheen_parm, float clearcoat_parm, float anisotropic_parm) :
        clr(color_parm),
        roughness(roughness_parm),
        reflectance(reflectance_parm),
        metallic(metallic_parm),
        sheen(sheen_parm),
        clearcoat(clearcoat_parm),
        anisotropic(anisotropic_parm),
        mat_id(id)
    {
    }

public:
    int mat_id;
    bool islight;
    vec3 normal;

    vec3 clr;

    float roughness;
    vec3 reflection_color;

    float refraction;
    float refraction_roughness;
    vec3 refraction_color;

    float metallic;
    float reflectance;
    float sheen;
    float clearcoat;
    float clearcoatGloss;
    float anisotropic;
    float specularTint;
    float ior;
    float relativeIOR;
    float flatness;
    float specTrans;
};


OIIO::ImageBuf load_texture(const char* path)
{
    OIIO::ImageBuf tex(path);
    return tex;
}

vec3 get_tex_pixel(OIIO::ImageBuf& tex, int x, int y)
{
    float pixel[3];
    tex.OIIO::ImageBuf::getpixel(x, y, 0, pixel);
    return vec3(pixel[0], pixel[1], pixel[2]);
}

#endif