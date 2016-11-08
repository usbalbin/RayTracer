#pragma once

#include "stdafx.h"
#include "Meshes.hpp"

#include "glm/gtx/rotate_vector.hpp"

#define PI 3.141592653589793
//#define EPSILON 1.0e-6

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
	const float reflectFactor = 0.25f;
	const float refractFactor = 0.0f;

	std::vector<Vertex> invertedCubeVertices{
		Vertex(float3(-size, -size, -size), float4(0.0f, 0.0f, 0.0f, 1.0f), float3(0.0f,  0.0f, +1.0f), reflectFactor, refractFactor), Vertex(float3(+size, -size, -size), float4(1.0f, 0.0f, 0.0f, 1.0f), float3(0.0f, 0.0f,  +1.0f), reflectFactor, refractFactor), Vertex(float3(+size, +size, -size), float4(1.0f, 1.0f, 0.0f, 1.0f), float3(0.0f,  0.0f, +1.0f), reflectFactor, refractFactor), Vertex(float3(-size, +size, -size), float4(0.0f, 1.0f, 0.0f, 1.0f), float3(0.0f,  0.0f, +1.0f), reflectFactor, refractFactor), //front
		Vertex(float3(-size, -size, +size), float4(0.0f, 0.0f, 1.0f, 1.0f), float3(0.0f,  0.0f, -1.0f), reflectFactor, refractFactor), Vertex(float3(+size, -size, +size), float4(1.0f, 0.0f, 1.0f, 1.0f), float3(0.0f,  0.0f, -1.0f), reflectFactor, refractFactor), Vertex(float3(+size, +size, +size), float4(1.0f, 1.0f, 1.0f, 1.0f), float3(0.0f,  0.0f, -1.0f), reflectFactor, refractFactor), Vertex(float3(-size, +size, +size), float4(0.0f, 1.0f, 1.0f, 1.0f), float3(0.0f,  0.0f, -1.0f), reflectFactor, refractFactor), //back

		Vertex(float3(-size, -size, -size), float4(0.0f, 0.0f, 0.0f, 1.0f), float3(+1.0f,  0.0f,  0.0f), reflectFactor, refractFactor), Vertex(float3(-size, -size, +size), float4(0.0f, 0.0f, 1.0f, 1.0f), float3(+1.0f,  0.0f,  0.0f), reflectFactor, refractFactor), Vertex(float3(-size, +size, +size), float4(0.0f, 1.0f, 1.0f, 1.0f), float3(+1.0f,  0.0f,  0.0f), reflectFactor, refractFactor), Vertex(float3(-size, +size, -size), float4(0.0f, 1.0f, 0.0f, 1.0f), float3(+1.0f,  0.0f,  0.0f), reflectFactor, refractFactor), //left
		Vertex(float3(+size, -size, -size), float4(1.0f, 0.0f, 0.0f, 1.0f), float3(-1.0f,  0.0f,  0.0f), reflectFactor, refractFactor), Vertex(float3(+size, -size, +size), float4(1.0f, 0.0f, 1.0f, 1.0f), float3(-1.0f,  0.0f,  0.0f), reflectFactor, refractFactor), Vertex(float3(+size, +size, +size), float4(1.0f, 1.0f, 1.0f, 1.0f), float3(-1.0f,  0.0f,  0.0f), reflectFactor, refractFactor), Vertex(float3(+size, +size, -size), float4(1.0f, 1.0f, 0.0f, 1.0f), float3(-1.0f,  0.0f,  0.0f), reflectFactor, refractFactor), //right

		Vertex(float3(-size, -size, -size), float4(0.0f, 0.0f, 0.0f, 1.0f), float3(0.0f, +1.0f,  0.0f), reflectFactor, refractFactor), Vertex(float3(+size, -size, -size), float4(1.0f, 0.0f, 0.0f, 1.0f), float3(0.0f, +1.0f,  0.0f), reflectFactor, refractFactor), Vertex(float3(+size, -size, +size), float4(1.0f, 0.0f, 1.0f, 1.0f), float3(0.0f, +1.0f,  0.0f), reflectFactor, refractFactor), Vertex(float3(-size, -size, +size), float4(0.0f, 0.0f, 1.0f, 1.0f), float3(0.0f, +1.0f,  0.0f), reflectFactor, refractFactor), //bottom
		Vertex(float3(-size, +size, -size), float4(0.0f, 1.0f, 0.0f, 1.0f), float3(0.0f, -1.0f,  0.0f), reflectFactor, refractFactor), Vertex(float3(+size, +size, -size), float4(1.0f, 1.0f, 0.0f, 1.0f), float3(0.0f, -1.0f,  0.0f), reflectFactor, refractFactor), Vertex(float3(+size, +size, +size), float4(1.0f, 1.0f, 1.0f, 1.0f), float3(0.0f, -1.0f,  0.0f), reflectFactor, refractFactor), Vertex(float3(-size, +size, +size), float4(0.0f, 1.0f, 1.0f, 1.0f), float3(0.0f, -1.0f,  0.0f), reflectFactor, refractFactor)  //top
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

//No normal support
/*


std::vector<Vertex> smallCubeVertices = {
Vertex(float3(-side, -side, +side), float4(0.0f, 0.0f, 1.0f, 1.0f)),
Vertex(float3(+side, -side, +side), float4(1.0f, 0.0f, 1.0f, 1.0f)),
Vertex(float3(+side, +side, +side), float4(1.0f, 1.0f, 1.0f, 1.0f)),
Vertex(float3(-side, +side, +side), float4(0.0f, 1.0f, 1.0f, 1.0f)),
Vertex(float3(-side, -side, -side), float4(0.0f, 0.0f, 0.0f, 1.0f)),
Vertex(float3(+side, -side, -side), float4(1.0f, 0.0f, 0.0f, 1.0f)),
Vertex(float3(+side, +side, -side), float4(1.0f, 1.0f, 0.0f, 1.0f)),
Vertex(float3(-side, +side, -side), float4(0.0f, 1.0f, 0.0f, 1.0f))
};

std::vector<TriangleIndices> cubeTriangleIndices = {
TriangleIndices(0, 1, 2), TriangleIndices(2, 3, 0),
TriangleIndices(3, 2, 6), TriangleIndices(6, 7, 3),
TriangleIndices(7, 6, 5), TriangleIndices(5, 4, 7),
TriangleIndices(4, 0, 3), TriangleIndices(3, 7, 4),
TriangleIndices(0, 1, 5), TriangleIndices(5, 4, 0),
TriangleIndices(1, 5, 6), TriangleIndices(6, 2, 1)
};*/


std::vector<Vertex> genSphereVertices(float radius, float4 color, int qualityFactor) {
	const float reflectFactor = 0.99f;
	const float refractFactor = 0.0f;

	std::vector<Vertex> vertices;
	const float twoPi = 2.0f * (float)PI;
	const float halfPi = PI / 2.0f;

	float step = PI / qualityFactor;
	for (float pitch = 0; pitch < twoPi; pitch += step) {
		for (float yaw = -halfPi; yaw <= halfPi + 1.0e-6; yaw += step) {

			float3 position = glm::rotateY(glm::rotateX(float3(0, radius, 0), pitch), yaw);
			vertices.push_back(Vertex(position, color, normalize(position), reflectFactor, refractFactor));
		}

	}
	return vertices;
}

std::vector<TriangleIndices> genSphereIndices(int qualityFactor) {
	int vertexCount = (qualityFactor + 1) * 2 * qualityFactor;
	std::vector<TriangleIndices> indices;
	for (int i = 0; i < vertexCount; i++) {
		indices.push_back(TriangleIndices(
			i,
			(i - 1 - qualityFactor + vertexCount) % vertexCount,
			(i - 1 + vertexCount) % vertexCount
		));

		indices.push_back(TriangleIndices(
			i,
			(i - qualityFactor + vertexCount) % vertexCount,
			(i - 1 - qualityFactor + vertexCount) % vertexCount
		));
	}
	return indices;
}