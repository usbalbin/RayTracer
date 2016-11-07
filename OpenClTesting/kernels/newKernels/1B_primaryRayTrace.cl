#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable

#include "kernels/containers.h"
#include "kernels/intersection.h"



bool traceBruteForceColor(int objectCount, global const Object* allObjects, global const TriangleIndices* allTriangles, global const Vertex* allVertices, Ray ray, Vertex* intersectionPoint);
Hit sky(Ray ray);



#define gid get_global_id(0)

float4 testColor();

void kernel primaryRayTrace(
	int objectCount,
	global const Object* objects,
	global const TriangleIndices* triangles,
	global const Vertex* vertices,
	global const Ray* rays,
	global Hit* hits
){
	Vertex intersectionPoint;
	Ray ray = rays[gid];
	Hit hit;
	
	bool wasHit = traceBruteForceColor(objectCount, objects, triangles, vertices, ray, &intersectionPoint);
	if(wasHit){
		hit.vertex = intersectionPoint;
		hit.ray = ray;
	}
	else{
		hit = sky(ray);
	}
	
	hits[gid] = hit;
}

Hit sky(Ray ray){
	Hit hit;
	hit.ray = ray;
	hit.vertex.color = (float4)(1.0f, 0.41f, 0.71f, 1.0f);
	hit.vertex.reflectFactor = 0.0f;
	hit.vertex.refractFactor = 0.0f;
	return hit;
}