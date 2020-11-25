# Romano Render

Monte Carlo PathTracer
My first project using C++, the goal of this is to learn both this language, a bit of software engineering and more about rendering, maths and computer graphics.

Features list :
- Intel embree3 for fast path tracing
- Square, directional, dome and point lights
- Direct and indirect illumination
- Lambert material
- Dielectric material
- Simple reflective material
- OpenImageIO for output (jpg, exr, png, bmp)
- Multithreaded rendering
- Obj format support
- Batch and (very simple) progressive rendering

Planned :
- Improved camera model with physically based parameters
- Image-based lighting
- Uber-shader
- Adaptive sampling
- OpenGL view with controls to start, pause, restart render, change render settings, load a different scene, change materials, change light settings
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
