// OpenClTesting.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "OpenClRayTracer.hpp"
#include "OpenClContexts.hpp"
#include "OpenGlShaders.hpp"
#include "Meshes.hpp"

int main()
{
	int width = 1024;
	int height = 768;


	int maxInstanceCount = 4;
	int maxTotalVertexCount = 54;
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

		openClRayTracer.reserve(4, 22);
		openClRayTracer.autoResizeObjectTypes();
		openClRayTracer.writeToObjectTypeBuffers();
	}

	//auto cubeIndices = openClRayTracer.getTriangles(cubeTypeBuilder);// Doing stuff to this object type will alter every instance of this object type once the buffers are updated
	//auto cubeVertices = openClRayTracer.getVertices(cubeTypeBuilder);


	float v = 0;//3.14159265f / 1.0f;
	while (!glfwWindowShouldClose(window)) {
		float16 cameraMatrix{
			+cos(v), +0, +sin(v), +0,
			+0, +1, +0, +0,
			-sin(v), +0, +cos(v), +0,
			+0, +0, +0, +1
		};
		openClRayTracer.clear();
		openClRayTracer.push_back(Instance(triLowerTypeBuilder));
		openClRayTracer.push_back(Instance(triTypeBuilder));
		openClRayTracer.push_back(Instance(invertedCubeTypeBuilder));
		openClRayTracer.push_back(Instance(cubeTypeBuilder));
		
		

		
		cl::Event event;
		event = openClRayTracer.prepRayTraceNonBlocking();
		event.wait();
		
		openClRayTracer.rayTrace(cameraMatrix);
		v += 1e-2f;
	}
	//system("pause");
    return 0;
}