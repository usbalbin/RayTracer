#pragma once

#include "kernels/containers.h"
#include "kernels/intersection.h"


float4 traceRay(int objectCount, global const Object* objects, global const TriangleIndices* triangles, global const Vertex* vertices);
float4 traceBruteForce(Ray ray, int objectCount, global const Object* allObjects, global const TriangleIndices* allTriangles, global const Vertex* allVertices, Vertex* intersectionPoint);
float4 traceBruteForceColor(int objectCount, global const Object* objects, global const TriangleIndices* triangles, global const Vertex* vertices, Ray ray);

Vertex interpolateTriangle(Triangle triangle, float2 uv);
float4 interpolate4(float4 a, float4 b, float4 c, float2 uv);
float3 interpolate3(float3 a, float3 b, float3 c, float2 uv);


global const Vertex* getVertices(global const Vertex* allVertices, Object object);
global const TriangleIndices* getTrianglesIndices(global const TriangleIndices* allTriangles, Object object);
Triangle getTriangle2(global const Vertex* vertices, TriangleIndices triangleIndices);
Triangle getTriangle(global const TriangleIndices* trianglesIndices, global const Vertex* vertices, Object object, int index);


Ray genOrthogonalRay();
Ray genPerspectiveRay();

#include "kernels/rayTracer.cl"