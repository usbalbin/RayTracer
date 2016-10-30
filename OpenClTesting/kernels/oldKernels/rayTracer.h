#pragma once

#include "kernels/containers.h"
#include "kernels/intersection.h"


float4 traceRay(int objectCount, global const Object* objects, global const TriangleIndices* triangles, global const Vertex* vertices, float16 matrix);
float4 traceBruteForceColorOld(int objectCount, global const Object* objects, global const TriangleIndices* triangles, global const Vertex* vertices, Ray ray);

Vertex interpolateTriangleOld(Triangle triangle, float2 uv);
float4 interpolate4Old(float4 a, float4 b, float4 c, float2 uv);
float3 interpolate3Old(float3 a, float3 b, float3 c, float2 uv);


global const Vertex* getVerticesOld(global const Vertex* allVertices, Object object);
global const TriangleIndices* getTrianglesIndicesOld(global const TriangleIndices* allTriangles, Object object);
Triangle getTriangle2Old(global const Vertex* vertices, TriangleIndices triangleIndices);
Triangle getTriangleOld(global const TriangleIndices* trianglesIndices, global const Vertex* vertices, Object object, int index);


Ray genOrthogonalRayOld();
Ray genPerspectiveRayOld(float16 matrix);

#include "kernels/oldKernels/rayTracer.cl"