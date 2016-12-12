// OpenClTesting.cpp : Defines the entry point for the console application.
//




#include "stdafx.h"

#include <iostream>
#include <chrono>
#include <algorithm>

#include "OpenClRayTracer.hpp"
#include "OpenClContexts.hpp"
#include "OpenGlShaders.hpp"
#include "Meshes.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "Utils.hpp"

int main()
{
	int width = 1440;//1024;
	int height = 800;//768;
	

	int maxInstanceCount = 1;//8; //Does not have to be accurate if openClRayTracer::autoResize() is called later
	int maxTotalVertexCount = 3;// 129; //Same here
	int maxObjectTypeCount = 4;
	int maxObjectTypeVertexCount = 54;

	OpenClRayTracer openClRayTracer(width, height, maxInstanceCount, maxTotalVertexCount);


	openClRayTracer.debugCl();



#ifdef ADVANCED_RENDERER
	openClRayTracer.initializeAdvancedRender();
#endif
	GLFWwindow* window = openClRayTracer.getWindow();
	//openClRayTracer.sizeofDebug();

	std::cout << "Finished initializing(mostly)..." << std::endl;

	MultiInstanceBuilder cubeBuilder;
	MultiInstanceBuilder invertedCubeBuilder;
	MultiInstanceBuilder sphereTypeBuilder;

	std::string bunnyPath = "content/test.obj";
	MultiInstanceBuilder bunnyBuilder;
	{
		std::vector<Vertex> bunnyVertices;
		std::vector<TriangleIndices> bunnyIndices;
		readObjFile(bunnyVertices, bunnyIndices, bunnyPath, 1.0f, 1.0f);


		bunnyBuilder = openClRayTracer.push_backMultiObjectTypes(bunnyIndices, bunnyVertices, 128, 128);
		

		
		const int qualityFactor = 15;
		sphereTypeBuilder = openClRayTracer.push_backMultiObjectTypes(genSphereIndices(qualityFactor), genSphereVertices(0.45f, float4(0.95f, 0.95f, 0.95f, 1.0f), qualityFactor), 128, 128);
		
		

		invertedCubeBuilder = openClRayTracer.push_backMultiObjectTypes(genInvertedCubeIndices(), genInvertedCubeVertices(10));
		
		cubeBuilder = openClRayTracer.push_backMultiObjectTypes(genCubeIndices(), genCubeVertices(0.5f));
		
		openClRayTracer.reserve(maxInstanceCount, maxTotalVertexCount);
		openClRayTracer.autoResizeObjectTypes();
		openClRayTracer.writeToObjectTypeBuffers();
	}

	std::cout << "Finished initializing(completely)..." << std::endl;
	float v = 0;//3.14159265f / 1.0f;
	auto lastTime = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(window)) {
		auto nowTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float>(nowTime - lastTime).count();
		lastTime = nowTime;

		//Set up a matrix for the cameras position and orientation
		//Note!: Unlike OpenGL/Direct3D this matrix does not hold any perspective or projection information, 
		//ONLY the cameras position and orientation!
		float16 cameraMatrix = glm::rotate(float16(1.0f), v, float3(0, 1.0f, 0));
		//TODO make sure camera is centered when using identity matrix(kernel code in genPerspectiveRay() in rayTrace.cl)


		//Clear list of old stuff to drow from previous frame
		openClRayTracer.clear();

			
		

		openClRayTracer.push_back(MultiInstance(float16(1.0f), invertedCubeBuilder));
		openClRayTracer.push_back(MultiInstance(glm::translate(float16(4.0f), float3(-2.0f, 0.5f, 0)), sphereTypeBuilder));
		openClRayTracer.push_back(MultiInstance(glm::translate(float16(3.0f), float3(+3.0f, 0.5f, 0)), cubeBuilder));
		

		//openClRayTracer.push_back(Instance(
		//	glm::translate(float16(0.70f), float3(0.0f, 2.0f * sin(50.0f * v), 3.0f)),
		//	cubeTypeBuilder
		//));
		openClRayTracer.push_back(MultiInstance(
			glm::rotate(glm::translate(float16(20.0f), float3(0.0f, 0.0f, 0.0f)), v, float3(0, 1, 0)),
			bunnyBuilder
		));

		
		/*for (int z = 0; z < 1; z++)
			for (int y = 0; y < 1; y++)
				for (int x = 0; x < 0; x++)
					openClRayTracer.push_back(Instance(glm::translate(float16(1.0f), float3(-2.0f - x, y, z)), sphereTypeBuilder));
		*/
		//Resize buffers to fit the stuff to be drawn,
		//only needs to be callen when more/larger stuff has been added since last resize
		openClRayTracer.autoResize();//TODO: this is rather expensive, remove me if possible 
		




		//-----<Prepare and perform Ray Tracing>
		
		//Create event to know when we are done prepping
		cl::Event event;

		//Start the preparations where we will be transforming objects to world coordinates etc.
		event = openClRayTracer.prepRayTraceNonBlocking();

		//Whait for the preparations to finnish
		event.wait();
		
		//Start the actual Ray Tracing and draw result to the screen
		
#ifdef ADVANCED_RENDERER
		openClRayTracer.advancedRender(cameraMatrix);
#else
		openClRayTracer.rayTrace(cameraMatrix);
#endif
		//openClRayTracer.iterativeRayTrace(cameraMatrix);

		//-----</>


		//printf("sizeof(Instance): %d\n", sizeof(Instance));
		//Increment angle value

		const float turnRate = 0.5f;//Rads/sec
		const float maxTimeStep = 0.1f;
		v += turnRate * std::min(deltaTime, maxTimeStep);
	}
	//system("pause");
    return 0;
}