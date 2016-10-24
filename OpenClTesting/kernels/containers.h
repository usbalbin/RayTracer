#pragma once

//Always use largest type first in structs shared by host, also make sure they are defined AND aligned the same
/*
typedef union {
	struct {
		float4 color;
		float4 properties;
	};
	struct {
		float red; 
		float green;
		float blue;
		float padding;

		float refract;
		float reflect;
		float refractIndex;
		float reflectDuffusion;
	};
	
}Color;*/

typedef struct {
	float4 color;
	float3 normal;
	float3 position;
}Vertex;

typedef struct {
	float3 position;
	float3 direction;
	float3 inverseDirection;//inverseDirection = 1.0f / direction;
}Ray;

typedef struct {
	Vertex vertex;
	Ray ray;
} Hit;

typedef struct {
	float4 color;
	int reflectIndex;
	int refractIndex;
	float reflectFactor;
	float refractFactor;
} RayTree;

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

typedef struct {
	float16 modelMatrix;
	int meshType;
	int startVertex;
	int padding0, padding1;
} Instance;