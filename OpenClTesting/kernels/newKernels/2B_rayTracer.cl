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
Triangle getTriangle2(const Vertex* vertices, TriangleIndices triangleIndices);
Triangle getTriangle(const TriangleIndices* trianglesIndices, const Vertex* vertices, Object object, int index);



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
	
	local const TriangleIndices triangles[640];
	local const Vertex vertices[320];

	for (int objectIndex = 0; objectIndex < objectCount; objectIndex++) {
		
		Object object = allObjects[objectIndex];
		
		float nearDistacnce, farDistance;
		if (!work_group_any(intersectsBox(ray, object.boundingBox, &nearDistacnce, &farDistance)))/* || nearDistacnce >= closestTriangleDist))*/
			continue;
		
		
		
		
		event_t triEvent =  async_work_group_copy((local float*)triangles, (global float*)getTrianglesIndices(allTriangles, object), sizeof(TriangleIndices) / sizeof(float) * object.numTriangles, 0);
		event_t vertEvent = async_work_group_copy((local float*)vertices, (global float*)getVertices(allVertices, object), sizeof(Vertex) / sizeof(float) * object.numVertices, 0);
		
		wait_group_events(1, &triEvent);
		wait_group_events(1, &vertEvent);
		
		
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
	
	
	float3 lightDir = normalize((float3)(-0.9f, -0.5f, 0.2f));
	float dotProduct = dot(normalize((*intersectionPoint).normal), -lightDir);
	(*intersectionPoint).color *= dotProduct * 0.5f + 0.5f;									//Diffuse
	(*intersectionPoint).color += (float4)(1.0f) * max(pow(dotProduct, 51), 0.0f) * 0.25f;	//Specular
	
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

Triangle getTriangle2(const Vertex* vertices, TriangleIndices triangleIndices) {
	Triangle triangle;
	triangle.a = vertices[triangleIndices.a];
	triangle.b = vertices[triangleIndices.b];
	triangle.c = vertices[triangleIndices.c];
	return triangle;
}

Triangle getTriangle(const TriangleIndices* trianglesIndices, const Vertex* vertices, Object object, int index) {
	TriangleIndices triangleIndices = trianglesIndices[index];
	return getTriangle2(vertices, triangleIndices);
}