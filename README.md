# Romano Render

First of all, I'd like to thank everyone that helped and helps me on the Graphics Programming discord, it's an amazing community, that you can find here : https://discord.gg/2nyse8kpKb

This was my first project using C++, the goal of this was to learn both this language, software engineering, rendering, maths and computer graphics. I've decided to end this project here and start a new pathtracer, [RomanoRender++](https://github.com/romainaugier/RomanoRender2), to apply everything I've learned and create a more performant and versatile render engine, instead of rewriting almost everything here. I'll leave this project as it is, to have a memory of it but also compare to my new renderer and see how I've progressed.


# How To Build

RomanoRender uses [Vcpkg](https://github.com/microsoft/vcpkg) to manage libraries accross different platforms and [CMake](https://cmake.org/) to support cross-platforms building. 


Libraries needed are :
- [Embree 3](https://github.com/embree/embree)
- [OpenImageIO](https://github.com/OpenImageIO/oiio)
- [Boost 1.75.0](https://www.boost.org/)
- [OpenColorIO](https://opencolorio.org/)


Make sure Git is installed on your machine and run 
```shell
git clone https://github.com/microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat
```

Once Vcpkg is installed, run the following to integrate it within visual studio
```
.\vcpkg\vcpkg integrate install
```


Run the following to install the needed libraries. This might take up to 30 min to build everything (OpenImageIO has a lot of dependencies)
```
cd /path/to/vcpkg/installation
vcpkg install embree
vcpkg install OpenImageIO
vcpkg install Boost
vcpkg install OpenColorIO
```


Once all needed libraries have been installed, you can run CMake to build the project
```
mkdir build
cd build
cmake ../
cmake --build ./ --config Release
```


# Features

- Intel embree3 
- OpenColorIO support (default configuration shipped with the renderer is ACES 1.0.3)
- OpenImageIO support for image output (various format supported : exr, jpg, png, tiff...)
- Obj file format supported
- Different integrators (Pathtracing, Cartoon, Ambient Occlusion, Scene Viewer)
- Uber-Shader (with different layers : Lambertian BRDF, GGX/Cook-Torrance BRDF/BTDF, Random-Walk SSS)
- Rich render interface with a progressive render-view, scene editor (per scene object transform and material)
- Basic pinhole camera model with depth of field

# Planned 

- Importance Sampling / Multiple Importance Sampling
- More BxDFs
- Texture system 
- Small procedural texturing language
- Adaptive sampling
- Improve the ui
- Add support for Alembic file format
- Improve overall performance of the renderer


# Showcase

![UI](https://cdn.discordapp.com/attachments/714940749707214890/826372472748244992/unknown.png)


Journey:

First couple of triangles :
![Triangles](https://i.imgur.com/oIFx2VO.png)

First triangle mesh, exported from maya as obj :
![Sphere](https://i.imgur.com/C45rhSy.png)

First real mesh rendered with face normal as color, and a proper camera with position and lookat :
![Dino](https://i.imgur.com/yGJ5Vjh.png)

The first heavy mesh with a simple acceleration structure :
![Dragon1](https://i.imgur.com/1bffUnK.png)

The Pixar Kitchen rendered with an improved naive acceleration structure :
![Pxr Kitchen](https://imgur.com/fOvsSbw.png)

X Wing rendered with uv coordinates from the obj file :
![X_Wing](https://i.imgur.com/fBT3uXg.png)

First buggy test with light :
![Bug_light](https://i.imgur.com/ifPXk7a.png)

Finally getting a proper point light working :
![Pxr_Kitchen_pt_light](https://i.imgur.com/1NkyIHt.png)

Global illumination with point light :
![gi_dragon](https://imgur.com/BwdKWjW.png)

Buggy Pixar Kitchen render :
![Broken_pixar_gi](https://imgur.com/LrhkbOw.png)

Glass (and embree3 implementation) :
![Glass](https://imgur.com/qHnt7dH.png)

Few bugs through area light implementation :

![area_bug1](https://imgur.com/1GqX4Io.png)
![area_bug2](https://imgur.com/14AtQtl.png)

Finally working :

![first_area](https://imgur.com/C7Lpk8W.png)

A more interesting area light render :
![drag_area](https://imgur.com/fJC7Yzh.png)

Render mixing area and directional lights :
![area_and_dir](https://imgur.com/DAkX5yc.png)

Depth of field and pure reflective material :
![pure_reflec_dof](https://imgur.com/tvGHbuO.png)

The infamous white dome light aka ambien occlusion :
![dome_light_ao](https://imgur.com/6WSffH0.png)

GGX and Cook-Torrance brdf :
![ggx_brdf](https://imgur.com/j3RtwnK.png)

More GGX and Cook-Torrance :
![mustang_1](https://imgur.com/ptyFUyF.png)

GGX and Cook-Torrance for refraction :
![ggx_ct_refrac](https://imgur.com/xQalk0q.png)

HDRI naive lighting :

![hdri_1](https://imgur.com/lDZkYxh.png)

Hdri crazyness :
![hdri_mdr](https://imgur.com/628KbyT.png)

Combining all the cool stuff I implemented lately :
![gold_eagle](https://imgur.com/GjSkEqd.png)
(thank's to Flore Dechorgnat for the shading -> https://www.instagram.com/flore_dchrgnt)
![pxr_kitchen_lourd](https://cdn.discordapp.com/attachments/776720237218889738/791246572192923648/kitchen.jpg)

Random Walk SSS :
![rwsss_1](https://cdn.discordapp.com/attachments/776720237218889738/793155438119616553/sss_test5.jpg)

Some more Random Walk SSS:
![rwsss_2](https://cdn.discordapp.com/attachments/776720237218889738/792855883797364736/sss_test3.jpg)
