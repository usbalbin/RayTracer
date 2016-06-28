#pragma once


#include "glm\vec2.hpp"
#include "glm\vec3.hpp"
#include "glm\vec4.hpp"

//Always use largest type first in structs shared by openCL device, also make sure they are defined AND aligned the same

typedef glm::vec2 float2;
typedef glm::vec3 float3;
typedef glm::vec4 float4;

struct Vertex {
	Vertex() {}
	Vertex(float3 position) : position(position), color(0.0f){}
	Vertex(float3 position, float4 color) : position(position), color(color) {}
	float4 color;
	__declspec(align(4 * sizeof(float))) float3 position;
};

struct Ray {
	float3 position;
	float3 direction;
	float3 inverseDirection;//inverseDirection = 1.0f / direction;
};

struct Triangle {
	Triangle() {}
	Triangle(Vertex a, Vertex b, Vertex c) : a(a), b(b), c(c){}
	Vertex a, b, c;
};

struct TriangleIndices {
	TriangleIndices(int a, int b, int c) : a(a), b(b), c(c) {}
	int a, b, c;
};

struct AABB {
	AABB() {}
	AABB(float3 min, float3 max) : min(min), max(max) {}
	__declspec(align(4 * sizeof(float))) float3 min;
	__declspec(align(4 * sizeof(float))) float3 max;
};

struct Object {
	Object() {}
	AABB boundingBox;
	int startTriangle;
	int startVertex;
	int numTriangles;
	int numVertices;
};