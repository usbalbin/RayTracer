#pragma once

#include "Containers.hpp"
#include <vector>


std::vector<Vertex> genCubeVertices(float size);
std::vector<TriangleIndices> genCubeIndices();


std::vector<Vertex> genInvertedCubeVertices(float size);
std::vector<TriangleIndices> genInvertedCubeIndices();