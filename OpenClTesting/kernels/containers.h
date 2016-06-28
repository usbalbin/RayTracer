#pragma once

//Always use largest type first in structs shared by host, also make sure they are defined AND aligned the same


typedef struct {
	float4 color;
	float3 position;
}Vertex;

typedef struct {
	float3 position;
	float3 direction;
	float3 inverseDirection;//inverseDirection = 1.0f / direction;
}Ray;

typedef struct {
	Vertex a, b, c;
} Triangle;

typedef struct {
	int a, b, c;
} TriangleIndices;

typedef struct {
	float3 min;
	float3 max;
} AABB;

typedef struct {
	AABB boundingBox;
	int startTriangle;
	int startVertex;
	int numTriangles;
	int numVertices;
} Object;
