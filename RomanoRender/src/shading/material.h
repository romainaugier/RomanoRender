#pragma once
#include "utils/vec3.h"
#include <OpenImageIO/imagebuf.h>


#ifndef MATERIAL
#define MATERIAL


class Material
{
public:
    Material() {}

    Material(unsigned int &id) :
        mat_id(id)
    {
        //name = "material_" + std::to_string(id);
        clr = vec3(0.3f);
        roughness = 1.0f;
        refraction = 0.0f;
        islight = false;
        diffuse_roughness = 0.0f;
        specular = 0.0f;
        reflectance = 0.0f;
        metallic = 0.0f;
        sss = 0.0f;
        sss_radius = vec3(1.0f);
        sss_scale = 1.0f;
        sss_abs = 0.0f;
        sss_steps = 8;
        ior = vec3(1.3f);
        reflection_color = vec3(1.0f);
        specular_color = vec3(1.0f);
        refraction_color = vec3(1.0f);
        sss_color = vec3(1.0f);
    }

    Material(unsigned int& id, vec3 color_parm, vec3 n, bool is_light) :
        clr(color_parm),
        normal(n),
        islight(is_light)
    {        
    }

    Material(int& id, std::string _name, vec3 color_parm, float _roughness, float _refrac) :
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
        sss_radius = vec3(1.0f);
        sss_scale = 1.0f;
        sss_abs = 0.0f;
        sss_steps = 15;
        ior = vec3(1.0f);
        reflection_color = vec3(1.0f);
        specular_color = vec3(1.0f);
        refraction_color = vec3(1.0f);
        sss_color = vec3(1.0f);
    }


public:
    unsigned int mat_id;
    std::string name;
    bool islight;
    vec3 normal;

    vec3 clr;
    float diffuse_roughness;

    float roughness;
    vec3 reflection_color;

    float refraction;
    float refraction_roughness;
    vec3 refraction_color;

    float metallic;
    float reflectance;
    float specular;
    vec3 specular_color;
    vec3 ior;

    float sss;
    vec3 sss_color;
    vec3 sss_radius;
    float sss_scale;
    int sss_steps;
    float sss_abs;
};


#endif