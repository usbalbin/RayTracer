# RayTracer
Reverse Ray Tracer for rendering graphics using CPU, or GPU via OpenCL.

## TODO:
- Rename "Object" struct to "Mesh"

- Add support for multiple AABBs per mesh so there will be fewer triangles(expensive) to check for intersection per AABB(cheap) to improve performance.

- If possible, increase number of threads per Mesh in "vertex-shader"-kernel. There is currently only 1 thread / Mesh.

- Consider removing last run of the treeTraverser kernel with a modified version of the colorToPixel kernel.

- Consider moving AABB step into "vertex-shader" if beneficial

- Consider adding frustum culling step after AABB step

- Consider adding sorting step before Raytracer to improve performance

- Consider "chaining" OpenCL kernels to free up some CPU-time and to help making other calculations on CPU without it having to launch kernel steps during rendering of image.

- Look into what needs to be changed to support not only Windows but also Linux and Mac.
  - OpenGL - CL interop
  - get working directory for program.build()
  - etc.

## WIP

- Add support for reflection, refrection and light source(s) in some recursive-like fashion but implemented iterative through multiple kernels calls. One kernel for tracing rays and one for producing reflection/refraction rays on impacts. Repetedly call these
 multiple times and then collect the result through a third kernel.

  - Add some sort of "primary ray raytracer-kernel". This is only to be run for the primary rays and always results in having exactly width * height hits, so there will be a 1 - 1 hit to pixel mapping. The hits will also be sorted so that the hit corresponding to the pixel at (x, y) can be found by hit[y * width + x]. The hits that actually does not hit any geometry will get some sort of sky-values.
