# RayTracer
Reverse Ray Tracer for rendering graphics using CPU, or GPU via OpenCL.

## TODO:

- Add support for reflection, refrection and light source(s) in some recursive-like fashion but implemented iterative http://www.geeksforgeeks.org/inorder-tree-traversal-without-recursion/

- Add a vertex-shader like step before the actual ray tracing. This step should translate every vertex in each mesh by a 4x4 model matrix.

- Add AABB step between vertex-kernel and ray tracer for calculating the bounding box of every mesh after translation.

- Make sure the "vertex-shader" has support for instanced drawing where multiple meshes with different matrices are added that are using the same vertices. Maybe implement by having a constant or stack-like model buffer containing all different mesh types and one mesh buffer with mesh objects for each induvidual object to draw, containing only a model matrix and reference to its mesh type. The "vertex-shader" should translate every instance's mesh to its own vertices that are placed as input to the raytracer.