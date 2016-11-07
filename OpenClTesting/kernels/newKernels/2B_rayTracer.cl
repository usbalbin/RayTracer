#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable

#include "kernels/containers.h"
#include "kernels/intersection.h"



bool traceBruteForceColor(int objectCount, global const Object* allObjects, global const TriangleIndices* allTriangles, global const Vertex* allVertices, Ray ray, Vertex* intersectionPoint);
void summarizeHits(global Hit* results, volatile global int* globalResultCount, Hit result, bool hasResult);//, local atomic_int* groupResultCount);
void summarizeHitsNew(global Hit* results, volatile global int* globalResultCount, Hit result, bool hasResult);

Vertex interpolateTriangle(Triangle triangle, float2 uv);
float4 interpolate4(float4 a, float4 b, float4 c, float2 uv);
float3 interpolate3(float3 a, float3 b, float3 c, float2 uv);
float interpolate1(float a, float b, float c, float2 uv);


global const Vertex* getVertices(global const Vertex* allVertices, Object object);
global const TriangleIndices* getTrianglesIndices(global const TriangleIndices* allTriangles, Object object);
Triangle getTriangle2(global const Vertex* vertices, TriangleIndices triangleIndices);
Triangle getTriangle(global const TriangleIndices* trianglesIndices, global const Vertex* vertices, Object object, int index);



Ray genOrthogonalRay();
Ray genPerspectiveRay(float16 matrix);






#define gid get_global_id(0)

float4 testColor();

void kernel rayTraceAdvanced(
	int objectCount,
	global const Object* objects,
	global const TriangleIndices* triangles,
	global const Vertex* vertices,
	global const Ray* rays,
	global Hit* hits,
	volatile global int* hitCount
){
	Vertex intersectionPoint;
	Ray ray = rays[gid];
	Hit hit;
	
	bool wasHit = traceBruteForceColor(objectCount, objects, triangles, vertices, ray, &intersectionPoint);
	hit.vertex = intersectionPoint;
	hit.ray = ray;
	
	//TODO: check if this is even needed, it's currently done on host
	if(get_global_id(0)==0){																			// First worker will initialize groupResultCount to 0
        //printf("Before init hitCount:");// %d", atomic_load(hitCount));
		//atomic_init(hitCount, 0);
		*hitCount = 0;
		//printf("After init hitCount:");// %d", atomic_load(hitCount));
    }
	barrier(CLK_GLOBAL_MEM_FENCE);
	
	/*local atomic_int groupResultCount;
	if(get_local_id(0)==0){																			// First worker will initialize groupResultCount to 0
        atomic_init(&groupResultCount, 0);
    }
    barrier(CLK_GLOBAL_MEM_FENCE);
	summarizeHits(hits, hitCount, hit, wasHit, &groupResultCount);
	*/
	
	/*printf(
		"In Ray: pos = %2.6v3hlf, dir = %2.1v3hlf\n",
		hit.ray.position, hit.ray.direction
	);*/
	
	summarizeHits(hits, hitCount, hit, wasHit);
}




bool traceBruteForceColor(int objectCount, global const Object* allObjects, global const TriangleIndices* allTriangles, global const Vertex* allVertices, Ray ray, Vertex* intersectionPoint) {
	float closestTriangleDist = FLT_MAX;
	Triangle closestTriangle;
	float2 closestUv;

	for (int objectIndex = 0; objectIndex < objectCount; objectIndex++) {
		
		Object object = allObjects[objectIndex];
		
		float nearDistacnce, farDistance;
		if (!intersectsBox(ray, object.boundingBox, &nearDistacnce, &farDistance))
			continue;
		
		global const TriangleIndices* private triangles = getTrianglesIndices(allTriangles, object);
		global const Vertex* private vertices = getVertices(allVertices, object);
		
		for (int triangleIndex = 0; triangleIndex < object.numTriangles; triangleIndex++) {
			Triangle triangle = getTriangle(triangles, vertices, object, triangleIndex);

			float distance;
			float2 uv;
			if (intersectsTriangle(ray, triangle, &distance, &uv) && distance < closestTriangleDist) {
				closestTriangleDist = distance;
				closestTriangle = triangle;
				closestUv = uv;
			}
		}
	}
	
	if(closestTriangleDist == FLT_MAX){
		//printf("Ray did not hit anything!!!");
		return false;
	}

	*intersectionPoint = interpolateTriangle(closestTriangle, closestUv);
	
	
	float3 lightDir = normalize((float3)(-0.9f, 0.5f, 0.2f));
	(*intersectionPoint).color *= dot((*intersectionPoint).normal, -lightDir) * 0.5f + 0.5f;
	
	//printf(" - Ray actually hit something!!!");
	return true;//(float4)((float)(int)closestTriangleDist);
}


void summarizeHits(global Hit* results, volatile global int* globalResultCount, Hit result, bool hasResult){//, local atomic_int* groupResultCount){
	
	int groupIndex;
	int privateIndex;
	
	

	
	/*if(hasResult){																					// Everyone with a result will reserve themselves an index
		privateIndex = atomic_fetch_add(groupResultCount, 1);
		//privateIndex = atomic_fetch_add_explicit(groupResultCount, 1,
		//					memory_order_relaxed, memory_scope_work_group);
	}
	barrier(CLK_LOCAL_MEM_FENCE);																	//Wait for everyone to reserve themselves a place before next step
    
	
    if(get_local_id(0)==(get_local_size(0)-1)){														// Last worker will commit groupResultCount to globalResultCount
        groupIndex = atomic_fetch_add(globalResultCount, atomic_load(groupResultCount));							//and fetch the group index, thus reserving a section of indices
		//groupIndex = atomic_fetch_add_explicit(globalResultCount, 1,								//for the group
		//					memory_order_relaxed, memory_scope_device);
    }
	barrier(CLK_LOCAL_MEM_FENCE);																	// Make sure everyone in each group waits until the group's 
	*/																								//groupIndex has been recieved
    if(hasResult){
		int index = atom_add/*atomic_fetch_add*/(globalResultCount, 1);
		results[index] = result;
		//printf("Current index: %d\n", index);
	}
		
	/*if(hasResult){
		results[groupIndex + privateIndex] = result;
	}*/
}

#if 0
void summarizeHitsNew(global Hit* results, volatile global int* globalResultCount, Hit result, bool hasResult){
	int groupIndex;
	int privateIndex;
	
	bool someInGroupHasResult = work_group_any(hasResult);
	
	if(someInGroupHasResult){
		bool allInGroupHasResult = work_group_all(hasResult);
		privateIndex = allInGroupHasResult ?
			get_local_id(0) :
			work_group_scan_exclusive_add(hasResult ? 1 : 0);
	}
	
	barrier(CLK_GLOBAL_MEM_FENCE);
	
	if(someInGroupHasResult){
		if(get_local_id(0) == get_local_size(0) - 1){
			int groupResultCount = privateIndex + (hasResult ? 1 : 0);
			groupIndex = atom_add/*atomic_fetch_add*/(globalResultCount, groupResultCount);
		}
	}
	
	if(hasResult){
		int index = groupIndex + privateIndex;
		results[index] = result;
	}
}
#endif

Vertex interpolateTriangle(Triangle triangle, float2 uv){
	Vertex result;
	result.position = interpolate3(triangle.a.position, triangle.b.position, triangle.c.position, uv);
	result.normal = interpolate3(triangle.a.normal, triangle.b.normal, triangle.c.normal, uv);
	result.color = interpolate4(triangle.a.color, triangle.b.color, triangle.c.color, uv);
	result.reflectFactor = interpolate1(triangle.a.reflectFactor, triangle.b.reflectFactor, triangle.c.reflectFactor, uv);
	result.refractFactor = interpolate1(triangle.a.refractFactor, triangle.b.refractFactor, triangle.c.refractFactor, uv);
	
	return result;
}

float4 interpolate4(float4 a, float4 b, float4 c, float2 uv){
	float bFactor = uv.x;
	float cFactor = uv.y;
	float aFactor = 1 - uv.x - uv.y;
	
	return a * aFactor + b * bFactor + c * cFactor;
}

float3 interpolate3(float3 a, float3 b, float3 c, float2 uv){
	float bFactor = uv.x;
	float cFactor = uv.y;
	float aFactor = 1 - uv.x - uv.y;
	
	return a * aFactor + b * bFactor + c * cFactor;
}

float interpolate1(float a, float b, float c, float2 uv){
	float bFactor = uv.x;
	float cFactor = uv.y;
	float aFactor = 1 - uv.x - uv.y;
	
	return a * aFactor + b * bFactor + c * cFactor;
}

global const Vertex* getVertices(global const Vertex* allVertices, Object object) {
	return &allVertices[object.startVertex];
}

global const TriangleIndices* getTrianglesIndices(global const TriangleIndices* allTriangles, Object object) {
	return &allTriangles[object.startTriangle];
}

Triangle getTriangle2(global const Vertex* vertices, TriangleIndices triangleIndices) {
	Triangle triangle;
	triangle.a = vertices[triangleIndices.a];
	triangle.b = vertices[triangleIndices.b];
	triangle.c = vertices[triangleIndices.c];
	return triangle;
}

Triangle getTriangle(global const TriangleIndices* trianglesIndices, global const Vertex* vertices, Object object, int index) {
	TriangleIndices triangleIndices = trianglesIndices[index];
	return getTriangle2(vertices, triangleIndices);
}