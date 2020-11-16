#pragma once
#include "vec3.h"
#include <OpenImageIO/imagebuf.h>


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

    material(int& id, vec3 color_parm, float roughness_parm, float refraction_roughness_parm) :
        clr(color_parm),
        roughness(roughness_parm),
        refraction_roughness(refraction_roughness_parm),
        mat_id(id)
    {
        has_clr_map = false;
        has_roughness_map = false;
        has_normal_map = false;
    }

    material(int& id, const char *color_map_parm, float roughness_parm) :
        clr_map(color_map_parm),
        roughness(roughness_parm),
        mat_id(id)
    {
        has_clr_map = true;
        has_roughness_map = false;
        has_normal_map = false;
        //clr_buffer readimage()
    }

    material(int& id, const char *color_map_parm, const char *roughness_map_parm) :
        clr_map(color_map_parm),
        roughness_map(roughness_map_parm),
        mat_id(id)
    {
        has_clr_map = true;
        has_roughness_map = true;
        has_normal_map = false;
        //clr_buffer readimage()
        //roughness_buffer readimage()
    }

    material(int& id, const char *color_map_parm, const char *roughness_map_parm, const char *normal_map_parm) :
        clr_map(color_map_parm),
        roughness_map(roughness_map_parm),
        normal_map(normal_map_parm),
        mat_id(id)
    {
        has_clr_map = true;
        has_roughness_map = true;
        has_normal_map = true;
        //clr_buffer readimage();
        //roughness_buffer readimage();
        //normal_buffer readimage();
    }

public:
    int mat_id;
    bool islight;
    vec3 normal;

    vec3 clr;
    bool has_clr_map;
    const char *clr_map;
    OIIO::ImageBuf clr_buffer;
    int clr_buf_w;
    int clr_buf_h;

    float roughness;
    bool has_roughness_map;
    const char *roughness_map;
    OIIO::ImageBuf roughness_buffer;
    int rgh_buf_w;
    int rgh_buf_h;

    float refraction_roughness;
    vec3 refraction_color;

    bool has_normal_map;
    const char *normal_map;
    OIIO::ImageBuf normal_buffer;
    int nml_buf_w;
    int nml_buf_h;

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