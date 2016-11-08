

#include "kernels/containers.h"
#include "kernels/math.h"

#define gid get_global_id(0)

Vertex transformVertex(float16 matrix, Vertex vertex);

void kernel vertexShader(
	//In
	global const Object* objectTypes,
	global const Instance* instances,
	global const Vertex* objectTypesVertices,

	//Out
	global Object* objects,
	global Vertex* vertices
){
	Instance instance = instances[gid];
	Object objectType = objectTypes[instance.meshType];
	float16 modelMatrix = instance.modelMatrix;

	/*if(gid == 0){
		printf("instances[1].startTriangle: %d\n", instances[1].startVertex);
	}*/
	
	Object mesh;
	mesh.startTriangle = objectType.startTriangle;
	mesh.startVertex = instance.startVertex;
	mesh.numTriangles = objectType.numTriangles;
	mesh.numVertices = objectType.numVertices;
	
	objects[gid] = mesh;

	for(int i = 0; i < objectType.numVertices; i++){
		vertices[mesh.startVertex + i] = 
			transformVertex(
				modelMatrix, 
				objectTypesVertices[objectType.startVertex + i]
			);
	}
}

Vertex transformVertex(float16 matrix, Vertex vertex){
	Vertex result = vertex;
	
	result.position = mulMatVec(
		matrix,
		(float4)(vertex.position, 1.0f)
	).xyz;
	
	result.normal = mulMatVec(
		matrix,
		(float4)(vertex.normal, 0.0f)
	).xyz;
	return result;
}
#include "kernels/containers.h"

#define gid get_global_id(0)

void kernel aabb(
	//In
	global const 
	Vertex* vertices,
	
	//In and Out
	global Object* objects
	
){
	
	
	Object mesh = objects[gid];
	float3 maxi, mini;
	
	AABB aabb = mesh.boundingBox;
	aabb.min = aabb.max = vertices[mesh.startVertex].position;
	
	for(int i = 1; i < mesh.numVertices; i++){
		aabb.min = min(aabb.min, vertices[mesh.startVertex + i].position);
		aabb.max = max(aabb.max, vertices[mesh.startVertex + i].position);
	}
	objects[gid].boundingBox = aabb;
	
}
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

kernel void debug(
	volatile global /*atomic_int*/int* counter
){

	//atomic_fetch_add(counter, 1);
	/*Vertex v;
	int reflectlLoc = (float*)&v.reflectFactor - (float*)&v.color;
	int refractlLoc = (float*)&v.refractFactor - (float*)&v.color;
	printf(
		"reflectlLoc: %d\n"
		"refractlLoc: %d\n",
		reflectlLoc,
		refractlLoc
	);*/

}
#include "kernels/math.h"

#define xIndex get_global_id(0)
#define yIndex get_global_id(1)
#define width get_global_size(0)
#define height get_global_size(1)

Ray genPerspectiveRay(float16 matrix);

void kernel perspectiveRayGenerator(float16 matrix, global Ray* rays) {
	rays[yIndex * width + xIndex] = genPerspectiveRay(matrix);
}

Ray genPerspectiveRay(float16 matrix) {
	Ray ray;
	
	
	ray.position = mulMatVec(matrix, (float4)(0.0f, 0.0f, 7.0f, 1.0f)).xyz;

	
	
	float sideToSide = (float)xIndex / width;
	float topToBottom = (float)yIndex / height;

	float3 tl = (float3)(-1.0f, +1.0f, -1.0f); float3 tr = (float3)(+1.0f, +1.0f, -1.0f);
	float3 bl = (float3)(-1.0f, -1.0f, -1.0f); float3 br = (float3)(+1.0f, -1.0f, -1.0f);

	ray.direction = normalize(
		mix(
			mix(tl, tr, sideToSide),
			mix(bl, br, sideToSide),
			topToBottom
		)
	);

	ray.direction = mulMatVec(matrix, (float4)(ray.direction, 0.0f)).xyz;
	
	ray.inverseDirection = 1.0f / ray.direction;
	return ray;
}
#include "kernels/containers.h"
#include "kernels/intersection.h"


Hit sky(Ray ray);
bool traceBruteForceColor(int objectCount, global const Object* allObjects, global const TriangleIndices* allTriangles, global const Vertex* allVertices, Ray ray, Vertex* intersectionPoint);


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


void summarizeRays(global Ray* results, volatile global atomic_int* globalResultCount, Ray result, bool hasResult, int* indexOut, volatile local atomic_int* groupResultCount);
void summarizeRaysNew(global Ray* results, volatile global atomic_int* globalResultCount, Ray result, bool hasResult, int* indexOut);

void kernel rayGenerator(
	global const Hit* hits,
	volatile global atomic_int* rayIndex,
	global Ray* raysOut,
	global RayTree* rayTrees
){
	RayTree rayTree;
	Hit hit = hits[gid];
	rayTree.color = hit.vertex.color;
	rayTree.reflectFactor = hit.vertex.reflectFactor;
	rayTree.refractFactor = hit.vertex.refractFactor;


	bool hasReflection = rayTree.reflectFactor > 0;
	bool hasRefraction = rayTree.refractFactor > 0;
	int reflectionIndex = -1;
	int refractionIndex = -1;
	
	
	
	Ray reflection = reflect(hit);
	Ray refraction = refract(hit);
	
	
	volatile local atomic_int groupResultCount;
	if(get_local_id(0)==0){																			// First worker will initialize groupResultCount to 0
        atomic_init(&groupResultCount, 0);
		//groupResultCount = 0;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
	summarizeRays(raysOut, rayIndex, reflection, hasReflection, &reflectionIndex, &groupResultCount);

	
	if(get_local_id(0)==0){																			// First worker will initialize groupResultCount to 0
        atomic_init(&groupResultCount, 0);
		//groupResultCount = 0;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
	summarizeRays(raysOut, rayIndex, refraction, hasRefraction, &refractionIndex, &groupResultCount);
	
	rayTree.reflectIndex = reflectionIndex;
	rayTree.refractIndex = refractionIndex;
	
	rayTrees[gid] = rayTree;
}



void summarizeRays(global Ray* results, volatile global atomic_int* globalResultCount, Ray result, bool hasResult, int* indexOut, volatile local atomic_int* groupResultCount){
	/*
	int groupIndex;
	int privateIndex;
	
	
	if(hasResult){																					// Everyone with a result will reserve themselves an index
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
																									//groupIndex has been recieved
    
	if(hasResult){
		int index = groupIndex + privateIndex;
		*indexOut = index;
		results[index] = result;
	}*/
	
	if(hasResult){
		int index = atomic_fetch_add(globalResultCount, 1);
		*indexOut = index;
		results[index] = result;
	}
	
}


void summarizeRaysNew(global Ray* results, volatile global atomic_int* globalResultCount, Ray result, bool hasResult, int* indexOut){
	int groupIndex;
	int privateIndex;
	
	bool someInGroupHasResult = work_group_any(hasResult);
	
	if(someInGroupHasResult){
		bool allInGroupHasResult = work_group_all(hasResult);
		privateIndex = allInGroupHasResult ?
			get_local_id(0) :
			work_group_scan_exclusive_add(hasResult ? 1 : 0);
		
		barrier(CLK_LOCAL_MEM_FENCE);
			
		if(get_local_id(0) == 0){
			int groupResultCount = privateIndex + (hasResult ? 1 : 0);
			groupIndex = atomic_fetch_add(globalResultCount, groupResultCount);
		}
	}
	
	
	barrier(CLK_LOCAL_MEM_FENCE);
	if(hasResult){
		int index = groupIndex + privateIndex;
		*indexOut = index;
		results[index] = result;
	}
}







void kernel treeTraverser(
	global RayTree* rayTrees,
	global read_only RayTree* childRayTrees
){
	int reflectIndex = rayTrees[gid].reflectIndex;
	int refractIndex = rayTrees[gid].refractIndex;
	
	float4 surfaceColor = rayTrees[gid].color;
	
	if(reflectIndex != -1){
		rayTrees[gid].color += rayTrees[gid].reflectFactor * childRayTrees[reflectIndex].color;
	}
	
	if(refractIndex != -1){
		rayTrees[gid].color += rayTrees[gid].refractFactor * childRayTrees[refractIndex].color;
		
	}
	/*
	printf("gid: %d\nreflectIndex: %d\nrefractIndex: %d\n\n",
		get_global_id(0),
		reflectIndex,
		refractIndex
	);*/
}
#define xIndex get_global_id(0)
#define yIndex get_global_id(1)
#define width get_global_size(0)
#define height get_global_size(1)


void kernel colorToPixel(
	global const RayTree* rayTrees,
	__write_only image2d_t output
){
	int2 pos = (int2)(xIndex, height - yIndex);
	float4 color = rayTrees[yIndex * width + xIndex].color;
	
	
	
	
	
	write_imagef(output, pos, color.xyzw);
}