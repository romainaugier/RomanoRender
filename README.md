# GenepiRender

Monte Carlo pathtracer

Features list :
- scene import (obj format only for now)
- multiple objects rendering
- support triangles and quads (converted to triangles)
- custom acceleration structure
- multithreaded rendering (tile-based system)
- multi format image export (png, exr, jpg...) using OpenImageIO library


Planned :
- direct and global illumination
- different light types (directionnal, point, area, image-based lighting)
- simple materials (lambertian, dielectrics)
- simple texture import (diffuse, roughness and normal maps)
- simple procedural texturing
- better acceleration structure



Progress:

24/08/2020

![Pxr Kitchen](https://imgur.com/fOvsSbw.png)
Pixar Kitchen, rendered in 45 seconds.

![X Wing](https://imgur.com/sKMp5yJ.png)

X-Wing, rendered in 5 seconds.
