#include "kernels/containers.h"

#define width get_global_size(0)
#define height get_global_size(1)

void kernel computeKernel(
	int objectCount,
	global const Object* objects,
	global const TriangleIndices* triangles,
	global const Vertex* vertices,
	__write_only image2d_t output,
	global int* out
){
	int2 pos = (int2)(get_global_id(0),get_global_id(1));
	
	//float4 color = (float4)(1, 0, 0, 1);
	//float4 color = (float4)(pos.x / 1024.0f, pos.y / 768.0f, 0, 1);
	
	//color = (float4)(pos.x / (float)width, pos.y / (float)height, 0.0f, 1.0f);
	
	//color = traceRay(objectCount, objects, triangles, vertices);
	write_imagef(output, pos, vertices[0].color);
	//if(pos.x == 0 && pos.y == 0)
	*out = objects[1].startVertex;;
}
