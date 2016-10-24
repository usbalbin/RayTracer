#include "kernels/containers.h"
#include "kernels/rayTracer.h"

#define width get_global_size(0)
#define height get_global_size(1)

global Object objects[1];
global TriangleIndices triangles[1];
global float3 vertices[3];


void kernel computeKernel(
	int objectCount,
	global Object* objects_,
	global TriangleIndices* triangles_,
	global float3* vertices_,
	__write_only image2d_t output
){
	int2 pos = (int2)(get_global_id(0),get_global_id(1));
	float4 color;
	
	if(pos.x == 0 && pos.y == 0){
		vertices[0] = (float3)(-1, -1, -2);
		vertices[1] = (float3)(1, 1, -2.1f);
		vertices[2] = (float3)(1, -1, -2);
	
		objects[0].boundingBox.min = (float3)(-1, -1, -2.1f);
		objects[0].boundingBox.max = (float3)(1, 1, -2);
		objects[0].startTriangle = 0;
		objects[0].startVertex = 0;
		objects[0].numTriangles = 1;
		objects[0].numVertices = 3;
		
		triangles[0] = 0;
		triangles[1] = 1;
		triangles[2] = 2;
	
	}
	barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
	
	//float4 color = (float4)(1, 0, 0, 1);
	//float4 color = (float4)(pos.x / 1024.0f, pos.y / 768.0f, 0, 1);
	
	//color = (float4)(pos.x / (float)width, pos.y / (float)height, 0.0f, 1.0f);
	
	color = traceRay(objectCount, objects, triangles, vertices);
	write_imagef(output, pos, color.xyzw);
}
