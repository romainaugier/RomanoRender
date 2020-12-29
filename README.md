# Romano Render

Monte Carlo PathTracer
My first project using C++, the goal of this is to learn both this language, software engineering and more about rendering, maths and computer graphics.

Features list :
- Intel embree3 for fast path tracing
- Square, directional, dome/hdri and point lights
- Direct and indirect illumination
- Basic shader with lambertian for the diffuse brdf and GGX/Cook-Torrance for the reflective brdf and the btdf
- Random Walk Subsurface Scattering
- OpenImageIO for output (jpg, exr, png, bmp)
- Multithreaded rendering
- Obj format support
- In progress UI with node based rendering and progressive renderview

Planned :
- Improved camera model with physically based parameters
- Multiple Importance Sampling
- Uber-shader
- Adaptive sampling
- Improve the ui
- Create a better scene parser and scene system
- Add support for Alembic format
- Improve overall performance of the renderer


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

