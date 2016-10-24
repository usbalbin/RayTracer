#pragma once

#include "Containers.hpp"
#include <vector>


std::vector<float4> cpuRayTrace(int width, int height, std::vector<Object> objects, std::vector<TriangleIndices> triangles, std::vector<Vertex> vertices);

float4 traceRay(int objectCount, /*global*/ const Object* objects, /*global*/ const TriangleIndices* triangles, /*global*/ const Vertex* vertices);
float4 traceBruteForce(int objectCount, /*global*/ const Object* objects, /*global*/ const TriangleIndices* triangles, /*global*/ const Vertex* vertices, Ray ray);

const Vertex* getVertices(const Vertex* allVertices, Object object);
const TriangleIndices* getTrianglesIndices(const TriangleIndices* allTriangles, Object object);
//Triangle getTriangle(const Vertex* vertices, const TriangleIndices triangleIndices);
Triangle getTriangle(const TriangleIndices* trianglesIndices, const Vertex* vertices, int index);
void processTriangles(const TriangleIndices* triangles, const Vertex* vertices, Object object);

Ray genOrthogonalRay();
Ray genPerspectiveRay();