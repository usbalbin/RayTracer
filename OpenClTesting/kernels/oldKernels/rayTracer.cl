#include "kernels/math.h"
#include "kernels/intersection.h"

#define xIndex get_global_id(0)
#define yIndex get_global_id(1)
#define width get_global_size(0)
#define height get_global_size(1)


float4 traceRay(int objectCount, global const Object* objects, global const TriangleIndices* triangles, global const Vertex* vertices, float16 matrix) {
	Ray ray = genPerspectiveRayOld(matrix);
	return traceBruteForceColorOld(objectCount, objects, triangles, vertices, ray);
}


Ray genOrthogonalRayOld() {
	Ray ray;
	float x = 2 * (float)xIndex / width - 1.0f;
	float y = 2 * (float)yIndex / height - 1.0f;
	float z = 0;

	ray.position = (float3)(x, y, z);
	ray.direction = (float3)(0.00001f, 0.00001f, -1);
	ray.inverseDirection = 1.0f / ray.direction;
	return ray;
}


Ray genPerspectiveRayOld(float16 matrix) {
	Ray ray;
	/*const float v = 3.14159265f / 4;

	float16 matrix = (float16)(
		+cos(v), +0, +sin(v), +0,
		+0, +1, +0, +0,
		-sin(v), +0, +cos(v), +0,
		+0, +0, +0, +1
	);*/
	
	ray.position = mulMatVec(matrix, (float4)(0.0f, 0.0f, 7.0f, 1.0f)).xyz;

	
	
	float sideToSide = (float)xIndex / width;
	float topToBottom = (float)yIndex / height;

	float3 tl = (float3)(-1.0f, +1.0f, -1.0f); float3 tr = (float3)(+1.0f, +1.0f, -1.0f);
	float3 bl = (float3)(-1.0f, -1.0f, -1.0f); float3 br = (float3)(+1.0f, -1.0f, -1.0f);

	ray.direction = normalize(
		mix(
			mix(tl, tr, sideToSide * (float)width / height),
			mix(bl, br, sideToSide * (float)width / height),
			topToBottom
		)
	);

	ray.direction = mulMatVec(matrix, (float4)(ray.direction, 0.0f)).xyz;
	
	ray.inverseDirection = 1.0f / ray.direction;
	return ray;
}


float4 traceBruteForceColorOld(int objectCount, global const Object* allObjects, global const TriangleIndices* allTriangles, global const Vertex* allVertices, Ray ray) {
	float4 result = (float4)(0, 0, 0, 1);
	
	float closestTriangleDist = FLT_MAX;
	Triangle closestTriangle;
	float2 closestUv;

	for (int objectIndex = 0; objectIndex < objectCount; objectIndex++) {
		
		Object object = allObjects[objectIndex];
		global const TriangleIndices* triangles = getTrianglesIndicesOld(allTriangles, object);
		global const Vertex* vertices = getVerticesOld(allVertices, object);
		
		float nearDistacnce, farDistance;
		if (!intersectsBox(ray, object.boundingBox, &nearDistacnce, &farDistance) || nearDistacnce >= closestTriangleDist)
			continue;
		
		result.x = 1;
		//return (float4)(1.0f, 0.0f, 0.0f, 1.0f);
		for (int triangleIndex = 0; triangleIndex < object.numTriangles; triangleIndex++) {
			Triangle triangle = getTriangleOld(triangles, vertices, object, triangleIndex);

			float distance;
			float2 uv;
			if (intersectsTriangle(ray, triangle, &distance, &uv) && distance < closestTriangleDist) {
				closestTriangleDist = distance;
				closestTriangle = triangle;
				closestUv = uv;
			}
		}

	}
	
	if(closestTriangleDist == FLT_MAX)
		return result;

	Vertex interpolated = interpolateTriangleOld(closestTriangle, closestUv);
	
	result = interpolated.color;
	
	float3 lightDir = normalize((float3)(-0.9f, 0.5f, 0.2f));
	result *= dot(interpolated.normal, -lightDir) * 0.5f + 0.5f;
	
	return result;//(float4)((float)(int)closestTriangleDist);
}

Vertex interpolateTriangleOld(Triangle triangle, float2 uv){
	Vertex result;
	result.position = interpolate3Old(triangle.a.position, triangle.b.position, triangle.c.position, uv);
	result.normal = interpolate3Old(triangle.a.normal, triangle.b.normal, triangle.c.normal, uv);
	result.color = interpolate4Old(triangle.a.color, triangle.b.color, triangle.c.color, uv);
	return result;
}

float4 interpolate4Old(float4 a, float4 b, float4 c, float2 uv){
	float bFactor = uv.x;
	float cFactor = uv.y;
	float aFactor = 1 - uv.x - uv.y;
	
	return a * aFactor + b * bFactor + c * cFactor;
}

float3 interpolate3Old(float3 a, float3 b, float3 c, float2 uv){
	float bFactor = uv.x;
	float cFactor = uv.y;
	float aFactor = 1 - uv.x - uv.y;
	
	return a * aFactor + b * bFactor + c * cFactor;
}

global const Vertex* getVerticesOld(global const Vertex* allVertices, Object object) {
	return &allVertices[object.startVertex];
}

global const TriangleIndices* getTrianglesIndicesOld(global const TriangleIndices* allTriangles, Object object) {
	return &allTriangles[object.startTriangle];
}


Triangle getTriangle2Old(global const Vertex* vertices, TriangleIndices triangleIndices) {
	Triangle triangle;
	triangle.a = vertices[triangleIndices.a];
	triangle.b = vertices[triangleIndices.b];
	triangle.c = vertices[triangleIndices.c];
	return triangle;
}

Triangle getTriangleOld(global const TriangleIndices* trianglesIndices, global const Vertex* vertices, Object object, int index) {
	TriangleIndices triangleIndices = trianglesIndices[index];
	return getTriangle2Old(vertices, triangleIndices);
}