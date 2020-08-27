#pragma once
#include "vec3.h"
#include <OpenImageIO/imagebuf.h>


class material
{
public:
    material() {}

    material(vec3 color_parm, float roughness_parm) :
        clr(color_parm),
        roughness(roughness_parm)
    {
        has_clr_map = false;
        has_roughness_map = false;
        has_normal_map = false;
    }

    material(const char *color_map_parm, float roughness_parm) :
        clr_map(color_map_parm),
        roughness(roughness_parm)
    {
        has_clr_map = true;
        has_roughness_map = false;
        has_normal_map = false;
        //clr_buffer readimage()
    }

    material(const char *color_map_parm, const char *roughness_map_parm) :
        clr_map(color_map_parm),
        roughness_map(roughness_map_parm)
    {
        has_clr_map = true;
        has_roughness_map = true;
        has_normal_map = false;
        //clr_buffer readimage()
        //roughness_buffer readimage()
    }

    material(const char *color_map_parm, const char *roughness_map_parm, const char *normal_map_parm) :
        clr_map(color_map_parm),
        roughness_map(roughness_map_parm),
        normal_map(normal_map_parm)
    {
        has_clr_map = true;
        has_roughness_map = true;
        has_normal_map = true;
        //clr_buffer readimage();
        //roughness_buffer readimage();
        //normal_buffer readimage();
    }

public:
    vec3 clr;
    bool has_clr_map;
    const char *clr_map;
    OIIO::ImageBuf clr_buffer;

    float roughness;
    bool has_roughness_map;
    const char *roughness_map;
    OIIO::ImageBuf roughness_buffer;

    bool has_normal_map;
    const char *normal_map;
    OIIO::ImageBuf normal_buffer;
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