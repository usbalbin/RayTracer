#pragma once

#include "Containers.hpp"
#include <vector>


std::vector<Vertex> genCubeVertices(float size);
std::vector<TriangleIndices> genCubeIndices();


std::vector<Vertex> genInvertedCubeVertices(float size);
std::vector<TriangleIndices> genInvertedCubeIndices();

std::vector<Vertex> genSphereVertices(float radius, float4 color, int qualityFactor);

std::vector<TriangleIndices> genSphereIndices(int qualityFactor);
