#pragma once

#include "glm\mat4x4.hpp"

#include "glm\vec2.hpp"
#include "glm\vec3.hpp"
#include "glm\vec4.hpp"

//Always use largest type first in structs shared by openCL device, also make sure they are defined AND aligned the same

typedef glm::mat4 float16;

typedef glm::vec2 float2;
typedef glm::vec3 float3;
typedef glm::vec4 float4;
/*
union Color {
	Color() {}
	Color(float3 color) : color(color, 0.0f), properties(0.0f, 0.0f, 1.0f, 0.0f) {}
	Color(float4 color) : color(color), properties(0.0f, 0.0f, 1.0f, 0.0f) {}
	Color(float3 color, float4 properties) : color(color, 0.0f), properties(properties) {}
	Color(float4 color, float4 properties) : color(color), properties(properties) {}
	Color(float3 color, float refraction, float reflection, float refractionIndex, float reflectiveDiffusion) : color(color, 0.0f) {
		this->refract = refraction;
		this->reflect = reflection;
		this->refractIndex = refractionIndex;
		this->reflectDuffusion = reflectiveDiffusion;
	}
	Color(float4 color, float refraction, float reflection, float refractionIndex, float reflectiveDiffusion) : color(color) {
		this->refract = refraction;
		this->reflect = reflection;
		this->refractIndex = refractionIndex;
		this->reflectDuffusion = reflectiveDiffusion;
	}


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
	
};

*/
struct Vertex {
	Vertex() {}
	Vertex(float3 position, float4 color) : position(position), color(color), normal(0.0f) {}
	Vertex(float3 position, float4 color, float3 normal) : position(position), color(color), normal(normal), reflectFactor(0.5f), refractFactor(0.25f) {}
	//Vertex(float3 position, Color color, float3 normal) : position(position), color(color), normal(normal) {}
	float4 color;
	__declspec(align(4 * sizeof(float))) float3 normal;
	__declspec(align(4 * sizeof(float))) float3 position;
	__declspec(align(4 * sizeof(float))) float reflectFactor;
	float refractFactor;
};

struct Ray {
	__declspec(align(4 * sizeof(float))) float3 position;
	__declspec(align(4 * sizeof(float))) float3 direction;
	__declspec(align(4 * sizeof(float))) float3 inverseDirection;//inverseDirection = 1.0f / direction;
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

struct InstanceBuilder : public Object {
	InstanceBuilder() {};
	InstanceBuilder(Object object, int meshType) {
		this->boundingBox = object.boundingBox;
		this->startTriangle = object.startTriangle;
		this->startVertex = object.startVertex;
		this->numTriangles = object.numTriangles;
		this->numVertices = object.numVertices;
		this->meshType = meshType;
	}
	int meshType;
};

struct Instance {//TODO make sure aligment is same in C++ and OpenCL -version of struct
	Instance() {};
	Instance(float16 modelMatrix, InstanceBuilder builder) : modelMatrix(modelMatrix), meshType(builder.meshType) {};

	float16 modelMatrix;
	int meshType;
	int startVertex;
	int padding[14];//TODO: try to find better solution to alignment issue
} ;

struct Hit {
	Vertex vertex;
	Ray ray;
};

struct RayTree {
	float4 color;
	int reflectIndex;
	int refractIndex;
	float reflectFactor;
	float refractFactor;
};