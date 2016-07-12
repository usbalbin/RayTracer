// OpenClTesting.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "OpenClRayTracer.hpp"
#include "OpenClContexts.hpp"
#include "OpenGlShaders.hpp"
#include "Meshes.hpp"
#include "glm\gtc\matrix_transform.hpp"

int main()
{
	int width = 1024;
	int height = 768;


	int maxInstanceCount = 8;
	int maxTotalVertexCount = 129;
	int maxObjectTypeCount = 4;
	int maxObjectTypeVertexCount = 54;


	OpenClRayTracer openClRayTracer(width, height, maxInstanceCount, maxTotalVertexCount);
	GLFWwindow* window = openClRayTracer.getWindow();
	//openClRayTracer.sizeofDebug();

	InstanceBuilder cubeTypeBuilder;
	InstanceBuilder invertedCubeTypeBuilder;
	InstanceBuilder triLowerTypeBuilder;
	InstanceBuilder triTypeBuilder;
	{
		std::vector<TriangleIndices> trianglesIndices = { TriangleIndices(0, 1, 2) };
		std::vector<Vertex> triangleVertices = { Vertex(float3(-1.0f, -1.0f, -2.0f), float4(1.0f, 0.0f, 0.0f, 1.0f)), Vertex(float3(1.0f, +1.0f, -2.1f), float4(0.0f, 1.0f, 0.0f, 1.0f)), Vertex(float3(1.0f, -1.0f, -2.0f), float4(0.0f, 0.0f, 1.0f, 1.0f)) };
		std::vector<Vertex> triangleVerticesLower = { Vertex(float3(-1.0f, +1.0f, -2.0f), float4(1.0f, 0.0f, 0.0f, 1.0f)), Vertex(float3(1.0f, -1.0f, -2.1f), float4(0.0f, 1.0f, 0.0f, 1.0f)), Vertex(float3(1.0f, +1.0f, -2.0f), float4(0.0f, 0.0f, 1.0f, 1.0f)) };

		


		triLowerTypeBuilder = openClRayTracer.push_backObjectType(trianglesIndices, triangleVerticesLower);
		triTypeBuilder = openClRayTracer.push_backObjectType(trianglesIndices, triangleVertices);
		
		invertedCubeTypeBuilder = openClRayTracer.push_backObjectType(genInvertedCubeIndices(), genInvertedCubeVertices(10));
		
		cubeTypeBuilder = openClRayTracer.push_backObjectType(genCubeIndices(), genCubeVertices(0.5f));
		//object = openClRayTracer.push_back(cubeTriangleIndices, smallCubeVertices);

		openClRayTracer.reserve(maxInstanceCount, maxTotalVertexCount);
		openClRayTracer.autoResizeObjectTypes();
		openClRayTracer.writeToObjectTypeBuffers();
	}

	//auto cubeIndices = openClRayTracer.getTriangles(cubeTypeBuilder);// Doing stuff to this object type will alter every instance of this object type once the buffers are updated
	//auto cubeVertices = openClRayTracer.getVertices(cubeTypeBuilder);


	float v = 0;//3.14159265f / 1.0f;
	while (!glfwWindowShouldClose(window)) {
		float16 cameraMatrix = float16(1.0f);/*{
			+cos(v), +0, +sin(v), +0,
			+0, +1, +0, +0,
			-sin(v), +0, +cos(v), +0,
			+0, +0, +0, +1
		};*/
		cameraMatrix = glm::rotate(float16(1.0f), v, float3(0, 1.0f, 0));


		openClRayTracer.clear();
		openClRayTracer.push_back(Instance(float16(1.0f), triLowerTypeBuilder));
		openClRayTracer.push_back(Instance(float16(1.0f), triTypeBuilder));
		openClRayTracer.push_back(Instance(glm::translate(float16(2.0f), float3(1.0f, -1.0f, -0.5f)), triTypeBuilder));
		openClRayTracer.push_back(Instance(float16(1.0f), invertedCubeTypeBuilder));
		openClRayTracer.push_back(Instance(float16(1.0f), cubeTypeBuilder));
		openClRayTracer.push_back(Instance(glm::translate(float16(1.2f), float3(0.0f, 2.0f, 1.5f)), cubeTypeBuilder));
		openClRayTracer.push_back(Instance(
			glm::translate(float16(0.70f), float3(0.0f, 2.0f * sin(50.0f * v), 3.0f)),
			cubeTypeBuilder
		));
		openClRayTracer.push_back(Instance(
			glm::rotate(glm::translate(float16(1.0f), float3(2.0f, 0.0f, 0.0f)), v * 3.5f, float3(0, 0, 1)),
			cubeTypeBuilder
		));

		
		cl::Event event;
		event = openClRayTracer.prepRayTraceNonBlocking();
		event.wait();
		
		openClRayTracer.rayTrace(cameraMatrix);
		v += 1e-2f;
	}
	//system("pause");
    return 0;
}