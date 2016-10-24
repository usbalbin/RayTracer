#include "stdafx.h"
#include "DebugRayTracer.hpp"

#include "intersection.hpp"
#include "glm\glm.hpp"

#include <iostream>

#include "GL\glew.h"
#include "GLFW\glfw3.h"

/*struct TriangleIndices {
	int a, b, c;//Indices relative to its objects first vertex
};

struct Triangle {
	float3 a, b, c;
};

struct AABB {
	float3 min;
	float3 max;
};

struct Object {
	AABB boundingBox;
	int startTriangle;
	int startVertex;
	int numTriangles;
	int numVertices;
};*/

int yIndex, xIndex;
int height, width;


std::vector<float4> cpuRayTrace(const int maxWidth_, const int maxHeight_, std::vector<Object> objects, std::vector<TriangleIndices> triangles, std::vector<Vertex> vertices) {
	width = maxWidth_;
	height = maxHeight_;
	std::vector<float4> res(width * height);

	for (yIndex = 0; yIndex < height; yIndex++) {
		for (xIndex = 0; xIndex < width; xIndex++) {
			res[yIndex * width + xIndex] = traceRay(objects.size(), objects.data(), triangles.data(), vertices.data());
		}
	}

	/*
	for (int row = maxHeight - 1; row >= 0; row--) {
		for (int column = 0; column < maxWidth; column++) {
			std::cout << res[row * maxWidth + column].x << " ";
		}
		std::cout << std::endl;
	}
	*/
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, res.data());

	return res;
}


float4 traceRay(int objectCount, /*global*/ const Object* objects, /*global*/ const TriangleIndices* triangles, /*global*/ const Vertex* vertices) {
	Ray ray = genPerspectiveRay();
	return traceBruteForce(objectCount, objects, triangles, vertices, ray);
}

Ray genOrthogonalRay() {
	Ray ray;
	float x = 2 * (float)xIndex / width - 1.0f;
	float y = 2 * (float)yIndex / height - 1.0f;
	float z = 0;

	ray.position = float3(x, y, z);
	ray.direction = float3(0.00001f, 0.00001f, -1);
	ray.inverseDirection = 1.0f / ray.direction;
	return ray;
}

//Org
/*Ray genPerspectiveRay() {
	Ray ray;

	ray.position = float3(0.0f, 0.0f, 0.0f);

	float sideToSide = (float)xIndex / width;
	float topToBottom = (float)yIndex / height;




	ray.direction = glm::normalize(
		glm::mix(
			mix(float3(-1.0f, +1.0f, -1.0f), float3(+1.0f, +1.0f, -1.0f), sideToSide),
			mix(float3(-1.0f, -1.0f, -1.0f), float3(+1.0f, -1.0f, -1.0f), sideToSide),
			topToBottom
		)
	);

	ray.inverseDirection = 1.0f / ray.direction;
	return ray;
}*/
Ray genPerspectiveRay() {
	Ray ray;

	ray.position = float3(0.0f, 0.0f, 0.0f);

	float sideToSide = (float)xIndex / width;
	float topToBottom = (float)yIndex / height;




	ray.direction = normalize(
		mix(
			mix(float3(-1.0f, +1.0f, -1.0f), float3(+1.0f, +1.0f, -1.0f), sideToSide),
			mix(float3(-1.0f, -1.0f, -1.0f), float3(+1.0f, -1.0f, -1.0f), sideToSide),
			topToBottom
		)
	);

	ray.inverseDirection = 1.0f / ray.direction;
	return ray;
}
float4 traceBruteForce(int objectCount, /*global*/ const Object* objects, /*global*/ const TriangleIndices* triangles, /*global*/ const Vertex* vertices, Ray ray) {
	float closestTriangleDist = FLT_MAX;
	Triangle closestTriangle;

	for (int objectIndex = 0; objectIndex < objectCount; objectIndex++) {
		Object object = objects[objectIndex];
		float nearDistacnce, farDistance;
		if (!intersectsBox(ray, object.boundingBox, &nearDistacnce, &farDistance))
			continue;


		for (int triangleIndex = 0; triangleIndex < object.numTriangles; triangleIndex++) {
			Triangle triangle = getTriangle(triangles, vertices, triangleIndex);

			float distance;
			float2 uv;
			if (intersectsTriangle(ray, triangle, &distance, &uv) && distance < closestTriangleDist) {
				closestTriangleDist = distance;
				closestTriangle = triangle;
			}
		}

	}

	float4 black = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	float4 white = (float4)(1.0f, 1.0f, 1.0f, 1.0f);
	return closestTriangleDist == FLT_MAX ? black : white;//(float4)((float)(int)closestTriangleDist);
}


/*global*/ const Vertex* getVertices(/*global*/ const Vertex* allVertices, Object object) {
	return &allVertices[object.startVertex];
}



Triangle getTriangle2(/*global*/ const Vertex* vertices, TriangleIndices triangleIndices) {
	Triangle triangle;
	triangle.a = vertices[triangleIndices.a];
	triangle.b = vertices[triangleIndices.b];
	triangle.c = vertices[triangleIndices.c];
	return triangle;
}

Triangle getTriangle(/*global*/ const TriangleIndices* trianglesIndices, /*global*/ const Vertex* vertices, int index) {
	TriangleIndices triangleIndices = trianglesIndices[index];
	return getTriangle2(vertices, triangleIndices);
}