#pragma once

#include "stdafx.h"
#include "Meshes.hpp"

std::vector<Vertex> genCubeVertices(float size) {
	std::vector<Vertex> cubeVertices{
		Vertex(float3(-size, -size, -size), float4(0.0f, 0.0f, 0.0f, 1.0f), float3(0.0f,  0.0f, -1.0f)), Vertex(float3(+size, -size, -size), float4(1.0f, 0.0f, 0.0f, 1.0f), float3(0.0f, 0.0f,  -1.0f)), Vertex(float3(+size, +size, -size), float4(1.0f, 1.0f, 0.0f, 1.0f), float3(0.0f,  0.0f, -1.0f)), Vertex(float3(-size, +size, -size), float4(0.0f, 1.0f, 0.0f, 1.0f), float3(0.0f,  0.0f, -1.0f)), //front
		Vertex(float3(-size, -size, +size), float4(0.0f, 0.0f, 1.0f, 1.0f), float3(0.0f,  0.0f, +1.0f)), Vertex(float3(+size, -size, +size), float4(1.0f, 0.0f, 1.0f, 1.0f), float3(0.0f,  0.0f, +1.0f)), Vertex(float3(+size, +size, +size), float4(1.0f, 1.0f, 1.0f, 1.0f), float3(0.0f,  0.0f, +1.0f)), Vertex(float3(-size, +size, +size), float4(0.0f, 1.0f, 1.0f, 1.0f), float3(0.0f,  0.0f, +1.0f)), //back

		Vertex(float3(-size, -size, -size), float4(0.0f, 0.0f, 0.0f, 1.0f), float3(-1.0f,  0.0f,  0.0f)), Vertex(float3(-size, -size, +size), float4(0.0f, 0.0f, 1.0f, 1.0f), float3(-1.0f,  0.0f,  0.0f)), Vertex(float3(-size, +size, +size), float4(0.0f, 1.0f, 1.0f, 1.0f), float3(-1.0f,  0.0f,  0.0f)), Vertex(float3(-size, +size, -size), float4(0.0f, 1.0f, 0.0f, 1.0f), float3(-1.0f,  0.0f,  0.0f)), //left
		Vertex(float3(+size, -size, -size), float4(1.0f, 0.0f, 0.0f, 1.0f), float3(+1.0f,  0.0f,  0.0f)), Vertex(float3(+size, -size, +size), float4(1.0f, 0.0f, 1.0f, 1.0f), float3(+1.0f,  0.0f,  0.0f)), Vertex(float3(+size, +size, +size), float4(1.0f, 1.0f, 1.0f, 1.0f), float3(+1.0f,  0.0f,  0.0f)), Vertex(float3(+size, +size, -size), float4(1.0f, 1.0f, 0.0f, 1.0f), float3(+1.0f,  0.0f,  0.0f)), //right

		Vertex(float3(-size, -size, -size), float4(0.0f, 0.0f, 0.0f, 1.0f), float3(0.0f, -1.0f,  0.0f)), Vertex(float3(+size, -size, -size), float4(1.0f, 0.0f, 0.0f, 1.0f), float3(0.0f, -1.0f,  0.0f)), Vertex(float3(+size, -size, +size), float4(1.0f, 0.0f, 1.0f, 1.0f), float3(0.0f, -1.0f,  0.0f)), Vertex(float3(-size, -size, +size), float4(0.0f, 0.0f, 1.0f, 1.0f), float3(0.0f, -1.0f,  0.0f)), //bottom
		Vertex(float3(-size, +size, -size), float4(0.0f, 1.0f, 0.0f, 1.0f), float3(0.0f, +1.0f,  0.0f)), Vertex(float3(+size, +size, -size), float4(1.0f, 1.0f, 0.0f, 1.0f), float3(0.0f, +1.0f,  0.0f)), Vertex(float3(+size, +size, +size), float4(1.0f, 1.0f, 1.0f, 1.0f), float3(0.0f, +1.0f,  0.0f)), Vertex(float3(-size, +size, +size), float4(0.0f, 1.0f, 1.0f, 1.0f), float3(0.0f, +1.0f,  0.0f))  //top
	};
	return cubeVertices;
}

std::vector<TriangleIndices> genCubeIndices() {
	std::vector<TriangleIndices> cubeIndices{
		TriangleIndices(2, 1, 0),    TriangleIndices(0, 3, 2),//front
		TriangleIndices(7, 4, 5),    TriangleIndices(5, 6, 7),//back

		TriangleIndices(11, 8,9),    TriangleIndices(9,10,11),//left
		TriangleIndices(14,13,12),   TriangleIndices(12,15,14),//right

		TriangleIndices(17,18,19),    TriangleIndices(19,16,17),//bottom
		TriangleIndices(22,21,20),    TriangleIndices(20,23,22)//top
	};

	return cubeIndices;
}



std::vector<Vertex> genInvertedCubeVertices(float size) {
	std::vector<Vertex> invertedCubeVertices{
		Vertex(float3(-size, -size, -size), float4(0.0f, 0.0f, 0.0f, 1.0f), float3(0.0f,  0.0f, +1.0f)), Vertex(float3(+size, -size, -size), float4(1.0f, 0.0f, 0.0f, 1.0f), float3(0.0f, 0.0f,  +1.0f)), Vertex(float3(+size, +size, -size), float4(1.0f, 1.0f, 0.0f, 1.0f), float3(0.0f,  0.0f, +1.0f)), Vertex(float3(-size, +size, -size), float4(0.0f, 1.0f, 0.0f, 1.0f), float3(0.0f,  0.0f, +1.0f)), //front
		Vertex(float3(-size, -size, +size), float4(0.0f, 0.0f, 1.0f, 1.0f), float3(0.0f,  0.0f, -1.0f)), Vertex(float3(+size, -size, +size), float4(1.0f, 0.0f, 1.0f, 1.0f), float3(0.0f,  0.0f, -1.0f)), Vertex(float3(+size, +size, +size), float4(1.0f, 1.0f, 1.0f, 1.0f), float3(0.0f,  0.0f, -1.0f)), Vertex(float3(-size, +size, +size), float4(0.0f, 1.0f, 1.0f, 1.0f), float3(0.0f,  0.0f, -1.0f)), //back

		Vertex(float3(-size, -size, -size), float4(0.0f, 0.0f, 0.0f, 1.0f), float3(+1.0f,  0.0f,  0.0f)), Vertex(float3(-size, -size, +size), float4(0.0f, 0.0f, 1.0f, 1.0f), float3(+1.0f,  0.0f,  0.0f)), Vertex(float3(-size, +size, +size), float4(0.0f, 1.0f, 1.0f, 1.0f), float3(+1.0f,  0.0f,  0.0f)), Vertex(float3(-size, +size, -size), float4(0.0f, 1.0f, 0.0f, 1.0f), float3(+1.0f,  0.0f,  0.0f)), //left
		Vertex(float3(+size, -size, -size), float4(1.0f, 0.0f, 0.0f, 1.0f), float3(-1.0f,  0.0f,  0.0f)), Vertex(float3(+size, -size, +size), float4(1.0f, 0.0f, 1.0f, 1.0f), float3(-1.0f,  0.0f,  0.0f)), Vertex(float3(+size, +size, +size), float4(1.0f, 1.0f, 1.0f, 1.0f), float3(-1.0f,  0.0f,  0.0f)), Vertex(float3(+size, +size, -size), float4(1.0f, 1.0f, 0.0f, 1.0f), float3(-1.0f,  0.0f,  0.0f)), //right

		Vertex(float3(-size, -size, -size), float4(0.0f, 0.0f, 0.0f, 1.0f), float3(0.0f, +1.0f,  0.0f)), Vertex(float3(+size, -size, -size), float4(1.0f, 0.0f, 0.0f, 1.0f), float3(0.0f, +1.0f,  0.0f)), Vertex(float3(+size, -size, +size), float4(1.0f, 0.0f, 1.0f, 1.0f), float3(0.0f, +1.0f,  0.0f)), Vertex(float3(-size, -size, +size), float4(0.0f, 0.0f, 1.0f, 1.0f), float3(0.0f, +1.0f,  0.0f)), //bottom
		Vertex(float3(-size, +size, -size), float4(0.0f, 1.0f, 0.0f, 1.0f), float3(0.0f, -1.0f,  0.0f)), Vertex(float3(+size, +size, -size), float4(1.0f, 1.0f, 0.0f, 1.0f), float3(0.0f, -1.0f,  0.0f)), Vertex(float3(+size, +size, +size), float4(1.0f, 1.0f, 1.0f, 1.0f), float3(0.0f, -1.0f,  0.0f)), Vertex(float3(-size, +size, +size), float4(0.0f, 1.0f, 1.0f, 1.0f), float3(0.0f, -1.0f,  0.0f))  //top
	};
	return invertedCubeVertices;
}
std::vector<TriangleIndices> genInvertedCubeIndices() {
	std::vector<TriangleIndices> invertedCubeIndices{
		TriangleIndices(0, 1, 2), TriangleIndices(2, 3, 0),//front
		TriangleIndices(5, 4, 7), TriangleIndices(7, 6, 5),//back

		TriangleIndices(9, 8,11), TriangleIndices(11, 10, 9),//left
		TriangleIndices(12,13,14), TriangleIndices(14,15,12),//right

		TriangleIndices(19,18,17), TriangleIndices(17,16,19),//bottom
		TriangleIndices(20,21,22), TriangleIndices(22,23,20)//top
	};
	return invertedCubeIndices;
}