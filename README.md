# RayTracer
Reverse Ray Tracer for rendering graphics using CPU or GPU via OpenCL.

## TODO:

- Add BVH structure support in advancedRayTracer - kernel, potentialy improving performance from n to log(n) with BVH
  - BVH for object/mesh lookup in advancedRayTracer - kernel
  - BVH for triangle lookup per object/mesh in advancedRayTracer - kernel

- Rename "Object" struct to "Mesh"

- Make sure there is CPU-compute support and remove depricated "DebugRayTracer"

- Fix the window resize-functionality, currently crashing when using advancedRender

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
  - system("pause")
  - etc.
  
- Look into improving how the pixel color are computed. Good information at this site: http://www.flipcode.com/archives/Raytracing_Topics_Techniques-Part_2_Phong_Mirrors_and_Shadows.shtml

- Optimize and clean up kernels
  - Add read_only,write_only,const and similar properties
  - Minimize reads and writes to global memory, for example by making a privat copy when accessing multiple times
  - Look into the possibility of optimizing traceBruteForceColor()
    - Look into if possible and beneficial to: Save the objects whose AABB were hit and only run intersectsTriangle() on the closest ones in some way.

- Optimize and clean up host code
  - When possible/beneficial change flags for cl::Buffers to as conservative ones as possible such as CL_MEM_READ_ONLY or CL_MEM_WRITE_ONLY.
  - Remove unnecessary reads/writes to cl:Buffers

## WIP

- Add support for reflection, refrection and light source(s) in some recursive-like fashion but implemented iterative through multiple kernels calls. One kernel for tracing rays and one for producing reflection/refraction rays on impacts. Repetedly call these
 multiple times and then collect the result through a third kernel.
