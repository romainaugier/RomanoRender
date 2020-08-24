# GenepiRender

Monte Carlo pathtracer

Features list :
- scene import (obj format only for now)
- support only triangles (plan to add a triangulate function to convert quads)
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
