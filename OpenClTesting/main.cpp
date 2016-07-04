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

	OpenGlShaders renderer(width, height, "shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");
	GLuint openGlTextureID = renderer.setupInteropScreen();

	OpenClContexts openClContexts;
	openClContexts.initializeInteropGpu();

	cl::Device device = openClContexts.getGpuDevice(0);
	cl::Context context = openClContexts.getGpuContext(0);

	int maxObjectCount = 1;
	int maxTriangleCount = 1;
	int maxVertexCount = 3;


	OpenClRayTracer openClRayTracer(width, height, context, device, maxObjectCount, maxTriangleCount, maxVertexCount, openGlTextureID);
	Object object;

	{
		std::vector<TriangleIndices> trianglesIndices = { TriangleIndices(0, 1, 2) };
		std::vector<Vertex> triangleVertices = { Vertex(float3(-1.0f, -1.0f, -2.0f), float4(1.0f, 0.0f, 0.0f, 1.0f)), Vertex(float3(1.0f, +1.0f, -2.1f), float4(0.0f, 1.0f, 0.0f, 1.0f)), Vertex(float3(1.0f, -1.0f, -2.0f), float4(0.0f, 0.0f, 1.0f, 1.0f)) };
		std::vector<Vertex> triangleVerticesLower = { Vertex(float3(-1.0f, +1.0f, -2.0f), float4(1.0f, 0.0f, 0.0f, 1.0f)), Vertex(float3(1.0f, -1.0f, -2.1f), float4(0.0f, 1.0f, 0.0f, 1.0f)), Vertex(float3(1.0f, +1.0f, -2.0f), float4(0.0f, 0.0f, 1.0f, 1.0f)) };

		const float4 green(0.0f, 1.0f, 0.0f, 1.0f);
		float side = 10;
		std::vector<Vertex> cubeVertices = {
			Vertex(float3(-side, -side, +side), float4(0.0f, 0.0f, 1.0f, 1.0f)),
			Vertex(float3(+side, -side, +side), float4(1.0f, 0.0f, 1.0f, 1.0f)),
			Vertex(float3(+side, +side, +side), float4(1.0f, 1.0f, 1.0f, 1.0f)),
			Vertex(float3(-side, +side, +side), float4(0.0f, 1.0f, 1.0f, 1.0f)),
			Vertex(float3(-side, -side, -side), float4(0.0f, 0.0f, 0.0f, 1.0f)),
			Vertex(float3(+side, -side, -side), float4(1.0f, 0.0f, 0.0f, 1.0f)),
			Vertex(float3(+side, +side, -side), float4(1.0f, 1.0f, 0.0f, 1.0f)),
			Vertex(float3(-side, +side, -side), float4(0.0f, 1.0f, 0.0f, 1.0f))
		};

		side = 0.5f;
		/*std::vector<Vertex> smallCubeVertices = {
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

		object = openClRayTracer.push_back(trianglesIndices, triangleVerticesLower);
		object = openClRayTracer.push_back(trianglesIndices, triangleVertices);
		
		object = openClRayTracer.push_back(genInvertedCubeIndices(), genInvertedCubeVertices(10));
		
		object = openClRayTracer.push_back(genCubeIndices(), genCubeVertices(0.5f));
		//object = openClRayTracer.push_back(cubeTriangleIndices, smallCubeVertices);

		openClRayTracer.resize();
		openClRayTracer.writeToBuffers();
	}

	auto trianglesIndices = openClRayTracer.getTriangles(object);
	auto vertices = openClRayTracer.getVertices(object);


	float v = 0;//3.14159265f / 1.0f;
	while (!glfwWindowShouldClose(renderer.getWindow())) {
		float16 matrix{
			+cos(v), +0, +sin(v), +0,
			+0, +1, +0, +0,
			-sin(v), +0, +cos(v), +0,
			+0, +0, +0, +1
		};
		//openClRayTracer.computeOnCPU();//compute();
		//renderer.draw();
		if(1)
			openClRayTracer.compute(matrix);
		else
			openClRayTracer.debug(matrix);
		renderer.draw();
		v += 1e-2f;
	}
	//system("pause");
    return 0;
}