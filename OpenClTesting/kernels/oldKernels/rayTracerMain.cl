#include "kernels/containers.h"
#include "kernels/oldKernels/rayTracer.h"

#define xIndex get_global_id(0)
#define yIndex get_global_id(1)
#define width get_global_size(0)
#define height get_global_size(1)

float4 testColor();

void kernel rayTracer(
	int objectCount,
	float16 matrix,
	global const Object* objects,
	global const TriangleIndices* triangles,
	global const Vertex* vertices,
	__write_only image2d_t output
){
	
	int2 pos = (int2)(xIndex, height - yIndex);
	float4 color;
	
	//if(pos.x < 15 || pos.x > width - 15 - 1)
		//return;
	
	//float4 color = (float4)(1, 0, 0, 1);
	//float4 color = (float4)(pos.x / 1024.0f, pos.y / 768.0f, 0, 1);
	
	
	color = traceRay(objectCount, objects, triangles, vertices, matrix);
	//color = testColor();
	write_imagef(output, pos, color.xyzw);
}


















float4 testColor(){
	float3 res = mix(
		mix((float3)(1, 0, 0), (float3)(0, 1, 0), xIndex / (float)width),
		mix((float3)(0, 0, 0), (float3)(0, 0, 1), xIndex / (float)width),
		yIndex / (float)height
	);
	return (float4)(res, 1);
}