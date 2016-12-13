#include "stdafx.h"

#include "OpenClRayTracer.hpp"
#include "OpenClContexts.hpp"
#include "DebugRayTracer.hpp"
#include "Utils.hpp"

#include "GLFW\glfw3.h"

#include <iostream>
#include <stack>
#include <atomic>
#include <iomanip>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif // _WIN32

OpenClRayTracer::OpenClRayTracer(int width, int height,
	int maxInstanceCount, int maxTotalVertexCount) : 
	renderer(width, height, "shaders/vertexShader.glsl", "shaders/fragmentShader.glsl",
		[](GLFWwindow* window, int width, int height) {
			OpenClRayTracer* rayTracer = (OpenClRayTracer*)glfwGetWindowUserPointer(window);

			rayTracer->resizeCallback(window, width, height);
		}) {
	
	glfwSetWindowUserPointer(renderer.getWindow(), this);

	this->width = width;
	this->height = height;
	this->resultImages.resize(1);
	this->openGlTextureID = renderer.setupScreen();
	initialize();
	reserve(maxInstanceCount, maxTotalVertexCount);
}

OpenClRayTracer::~OpenClRayTracer()
{
}

void OpenClRayTracer::initialize() {
	OpenClContexts openClContexts;
	

#ifndef RUN_ON_CPU
	openClContexts.initializeInteropGpu();
	cl::Device device = openClContexts.getGpuDevice(0);
	this->context = openClContexts.getGpuContext(0);
#else
	openClContexts.initializeCpu();
	cl::Device device = openClContexts.getCpuDevice(0);
	this->context = openClContexts.getCpuContext(0);
#endif
	


	cl::Program::Sources sources;

	std::string vertexShaderSource = readFileToString("kernels/vertexShader.cl");
	std::string aabbSource = readFileToString("kernels/aabb.cl");
	std::string rayTracerSource = readFileToString("kernels/oldKernels/rayTracer.cl");
	std::string rayTracerKernelSource = readFileToString("kernels/oldKernels/rayTracerMain.cl");
	std::string debugSource = readFileToString("kernels/debug.cl");
#ifdef ADVANCED_RENDERER
	std::string perspectiveRayGeneratorSource = readFileToString("kernels/newKernels/1_perspectiveRayGenerator.cl");
	std::string rayGeneratorSource = readFileToString("kernels/newKernels/2A_rayGenerator.cl");
	std::string rayTraceAdvancedSource = readFileToString("kernels/newKernels/2B_rayTracer.cl");
	std::string treeTraverserSource = readFileToString("kernels/newKernels/3_treeTraverser.cl");
	std::string colorToPixelSource = readFileToString("kernels/newKernels/4_colorToPixel.cl");
#endif



	sources.push_back({ vertexShaderSource.c_str(), vertexShaderSource.length() });
	sources.push_back({ aabbSource.c_str(), aabbSource.length() });
	//sources.push_back({ rayTracerSource.c_str(), rayTracerSource.length() });
	sources.push_back({ rayTracerKernelSource.c_str(), rayTracerKernelSource.length() });
	sources.push_back({ debugSource.c_str(), debugSource.length() });
#ifdef ADVANCED_RENDERER
	sources.push_back({ perspectiveRayGeneratorSource.c_str(), perspectiveRayGeneratorSource.length() });
	sources.push_back({ rayTraceAdvancedSource.c_str(), rayTraceAdvancedSource.length() });
	sources.push_back({ rayGeneratorSource.c_str(), rayGeneratorSource.length() });
	sources.push_back({ treeTraverserSource.c_str(), treeTraverserSource.length() });
	sources.push_back({ colorToPixelSource.c_str(), colorToPixelSource.length() });
#endif

	writeSourcesToFile(sources, "kernels/output/allKernels.cl");

	cl::Program program(context, sources);



	std::cout << "---------------- Compilation status ----------------" << std::endl;
	std::string compileMessage;
	char programPathBuffer[256];
	getcwd(programPathBuffer, 256);
	std::string programPath = programPathBuffer;
	std::string stuff = device.getInfo<CL_DEVICE_OPENCL_C_VERSION>();
	std::string supported_extensions = device.getInfo<CL_DEVICE_EXTENSIONS>();
	std::cout << supported_extensions << std::endl;


	std::cout << "Path: \"" << programPath << "\"" << std::endl;







	// KOLLA PÅ						-CL-STD=CL2.0












#ifdef ADVANCED_RENDERER
	std::string extraOptions = "-cl-std=CL2.2 "/*-s \"" + programPath + "kernels/newKernels/3_treeTraverser.cl\" "*/;
#else
	std::string extraOptions = "";// "-cl-std=CL2.0";// "-cl-std=c++";// "-cl-std=CL2.0";// "-cl-unsafe-math-optimizations -cl-fast-relaxed-math";
#endif
	std::string compilerFlags = /*-O0 -g*/" -I " + programPath + " " + extraOptions;
	std::cout << compilerFlags << std::endl;
	try {
		std::cout << "Build started..." << std::endl;
		program.build({ device }, compilerFlags.c_str());
	}catch(cl::Error e){
		glfwDestroyWindow(renderer.getWindow());
		std::cout << "Prepping error message..." << std::endl;
		compileMessage = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
		std::cout << "Failed to compile with status " << e.err() << ": " << compileMessage << std::endl;
		system("pause");
		exit(1);
	}
	
	compileMessage = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
	std::cout << compileMessage << std::endl;
	
	cl_int status = CL_SUCCESS;
	
	queue = cl::CommandQueue(context, device, NULL, &status);

	vertexShaderKernel = cl::Kernel(program, "vertexShader", &status);
	aabbKernel = cl::Kernel(program, "aabb", &status);
	rayTraceKernel = cl::Kernel(program, "rayTracer", &status);
	debugKernel = cl::Kernel(program, "debug", &status);


#ifdef ADVANCED_RENDERER
	perspectiveRayGeneratorKernel = cl::Kernel(program, "perspectiveRayGenerator", &status);
	rayTraceAdvancedKernel = cl::Kernel(program, "rayTraceAdvanced", &status);
	rayGeneratorKernel = cl::Kernel(program, "rayGenerator", &status);
	treeTraverserKernel = cl::Kernel(program, "treeTraverser", &status);
	colorToPixelKernel = cl::Kernel(program, "colorToPixel", &status);
#endif
if (status != CL_SUCCESS) {
	std::cout << "Failed to create kernels" << std::endl;
	exit(1);
}

#ifndef RUN_ON_CPU
resultImages[0] = cl::ImageGL(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, openGlTextureID, &status);
if (status != CL_SUCCESS) {
	std::cout << "Failed to create OpenCL image from OpenGL texture" << std::endl;
	exit(1);
}
#else
resultImages[0] = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float4) * width * height);
if (status != CL_SUCCESS) {
	std::cout << "Failed to create OpenCL image from OpenGL texture" << std::endl;
	exit(1);
}
#endif // !RUN_ON_CPU


}

void OpenClRayTracer::autoResize() {
	reserveArrays(objectInstances.size());
	reserveBuffers(objectInstances.size(), transformedVertexCount);
}

void OpenClRayTracer::autoResizeObjectTypes() {
	reserveObjectTypeBuffers(objectTypes.size(), objectTypeIndices.size(), objectTypeVertices.size());
}

void OpenClRayTracer::reserve(int maxInstanceCount, int maxTotalVertexCount) {
	reserveArrays(maxInstanceCount);
	reserveBuffers(maxInstanceCount, maxTotalVertexCount);
}

void OpenClRayTracer::reserveArrays(int maxInstanceCount) {
	this->objectInstances.reserve(maxInstanceCount);
}

void OpenClRayTracer::reserveObjectTypeBuffers(int maxObjectTypeCount, int maxObjectTypeTriangleCount, int maxObjectTypeVertexCount) {
	objectTypeBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Object) * maxObjectTypeCount);
	objectTypeIndexBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(TriangleIndices) * maxObjectTypeTriangleCount);
	objectTypeVertexBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Vertex) * maxObjectTypeVertexCount);
}

void OpenClRayTracer::reserveBuffers(int maxInstanceCount, int maxTotalVertexCount) {
	objectInstanceBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Instance) * maxInstanceCount);

	transformedObjectBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Object) * maxInstanceCount);
	transformedVertexBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Vertex) * maxTotalVertexCount);

}

void OpenClRayTracer::clear() {
	this->objectInstances.clear();
	this->transformedVertexCount = 0;
}

void OpenClRayTracer::push_back(Instance instance) {
	instance.startVertex = this->transformedVertexCount;

	Object objectType = this->objectTypes[instance.meshType];

	this->objectInstances.push_back(instance);
	this->transformedVertexCount += objectType.numVertices;
}

void OpenClRayTracer::push_back(MultiInstance multiInstance)
{
	for (auto& instance : multiInstance.instances)
		push_back(instance);
}

Instance OpenClRayTracer::pop_instance() {
	Instance instance = this->objectInstances.back();

	instance.startVertex = -1;

	Object objectType = this->objectTypes[instance.meshType];
	this->transformedVertexCount -= objectType.numVertices;
	this->objectInstances.pop_back();

	return instance;
}

InstanceBuilder OpenClRayTracer::push_backObjectType(std::vector<TriangleIndices>& objectTypeIndices, std::vector<Vertex>& objectTypeVertices) {
	Object objectType;
	//objectType.boundingBox = AABB(float3(-5), float3(5)); //TODO remove this line!!!
	objectType.startTriangle = this->objectTypeIndices.size();
	objectType.startVertex = this->objectTypeVertices.size();
	objectType.numTriangles = objectTypeIndices.size();
	objectType.numVertices = objectTypeVertices.size();

	this->objectTypes.push_back(objectType);
	this->objectTypeIndices.insert(std::end(this->objectTypeIndices), std::begin(objectTypeIndices), std::end(objectTypeIndices));
	this->objectTypeVertices.insert(std::end(this->objectTypeVertices), std::begin(objectTypeVertices), std::end(objectTypeVertices));

	const int meshType = objectTypes.size() - 1;
	InstanceBuilder instanceBuilder(objectType, meshType);

	return instanceBuilder;
}

MultiInstanceBuilder OpenClRayTracer::push_backMultiObjectTypes(std::vector<TriangleIndices>& objectTypeIndices, std::vector<Vertex>& objectTypeVertices, int maxVerticesPerObject, int maxIndicesPerObject)
{
	MultiInstanceBuilder multiInstanceBuilder;
	auto& instanceBuilders = multiInstanceBuilder.instanceBuilders;
	std::vector<std::vector<Vertex>> verticesCollections;
	std::vector<std::vector<TriangleIndices>> indicesCollections;

	verticesCollections.push_back(objectTypeVertices);
	indicesCollections.push_back(objectTypeIndices);
	

	for (int i = 0; i < verticesCollections.size();) {
		if (indicesCollections[i].size() < maxIndicesPerObject && verticesCollections[i].size() < maxVerticesPerObject) {//If small enough
			instanceBuilders.push_back(
				push_backObjectType(indicesCollections[i], verticesCollections[i])
			);
			i++;
		}
		else {
			verticesCollections.emplace_back();
			indicesCollections.emplace_back();
			splitMesh(verticesCollections[i], indicesCollections[i], verticesCollections.back(), indicesCollections.back());
		}
	}
	return multiInstanceBuilder;
}



void OpenClRayTracer::writeToObjectTypeBuffers() {
	if (queue.enqueueWriteBuffer(objectTypeBuffer, CL_TRUE, 0, sizeof(Object) * objectTypes.size(), objectTypes.data()) != CL_SUCCESS) {
		std::cout << "Failed to write to buffer" << std::endl;
		exit(1);
	}

	if (queue.enqueueWriteBuffer(objectTypeIndexBuffer, CL_TRUE, 0, sizeof(TriangleIndices) * objectTypeIndices.size(), objectTypeIndices.data()) != CL_SUCCESS) {
		std::cout << "Failed to write to buffer" << std::endl;
		exit(1);
	}

	if (queue.enqueueWriteBuffer(objectTypeVertexBuffer, CL_TRUE, 0, sizeof(Vertex) * objectTypeVertices.size(), objectTypeVertices.data()) != CL_SUCCESS) {
		std::cout << "Failed to write to buffer" << std::endl;
		exit(1);
	}
}

void OpenClRayTracer::writeToInstanceBuffer() {
	if (queue.enqueueWriteBuffer(objectInstanceBuffer, CL_TRUE, 0, sizeof(Instance) * objectInstances.size(), objectInstances.data()) != CL_SUCCESS) {
		std::cout << "Failed to write to buffer" << std::endl;
		exit(1);
	}
}

void OpenClRayTracer::rayTrace(float16 matrix) {
	rayTraceNonBlocking(matrix);// .wait();
	queue.finish();
	fetchRayTracerResult();
}


void OpenClRayTracer::computeOnCPU()
{
	//TODO make me work, or move me out of here
	//result = cpuRayTrace(width, height, objectTypes, triangles, objectTypeVertices);
}

//Also writes to instanceBuffer
cl::Event OpenClRayTracer::vertexShaderNonBlocking() {
	writeToInstanceBuffer();

	if (vertexShaderKernel.setArg(0, objectTypeBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	if (vertexShaderKernel.setArg(1, objectInstanceBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}

	if (vertexShaderKernel.setArg(2, objectTypeVertexBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}

	if (vertexShaderKernel.setArg(3, transformedObjectBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	if (vertexShaderKernel.setArg(4, transformedVertexBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}

	queue.finish();

	
	cl::Event event;

	queue.enqueueNDRangeKernel(vertexShaderKernel, cl::NullRange, cl::NDRange(objectInstances.size()), cl::NullRange, 0, &event);

	
	return event;

}

cl::Event OpenClRayTracer::aabbNonBlocking() {
	
	if (aabbKernel.setArg(0, transformedVertexBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	if (aabbKernel.setArg(1, transformedObjectBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}

	queue.finish();

	cl::Event event;

	queue.enqueueNDRangeKernel(aabbKernel, cl::NullRange, cl::NDRange(objectInstances.size()), cl::NullRange, 0, &event);

	return event;

}

//Give me a better name
cl::Event OpenClRayTracer::prepRayTraceNonBlocking() {
	vertexShaderNonBlocking().wait();
	return aabbNonBlocking();
}

cl::Event OpenClRayTracer::rayTraceNonBlocking(float16 matrix) {
	
	
	//Make sure OpenGL is done working
	glFinish();

#ifndef RUN_ON_CPU
	//Take ownership of OpenGL texture
	if (queue.enqueueAcquireGLObjects(&resultImages, NULL, NULL) != CL_SUCCESS) {
		std::cout << "Failed to acquire result Texture from OpenGL" << std::endl;
		exit(1);
	}
	queue.finish();//Make sure OpenCL has grabbed the texture from GL(probably not needed)
#endif // !1

	

	int instanceCount = objectInstances.size();
	if (rayTraceKernel.setArg(0, sizeof(instanceCount), &instanceCount) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	if (rayTraceKernel.setArg(1, sizeof(float16), &matrix) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	if (rayTraceKernel.setArg(2, transformedObjectBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	if (rayTraceKernel.setArg(3, objectTypeIndexBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	if (rayTraceKernel.setArg(4, transformedVertexBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	if (rayTraceKernel.setArg(5, resultImages[0]) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	
	queue.finish();

	cl::Event event;


	int status = queue.enqueueNDRangeKernel(rayTraceKernel, cl::NullRange, cl::NDRange(width, height), cl::NullRange, 0, &event);
	if (status != CL_SUCCESS) {
		std::cout << "Failed to enqueue rayTraceKernel with error: " << status << std::endl;
		system("pause");
		exit(1);
	}

	return event;
}



void OpenClRayTracer::debugCl() {
	int counter;

	counter = 0;

	cl::Buffer countBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(counter), &counter);


	if (debugKernel.setArg(0, countBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}

	cl::Event event;
	queue.enqueueNDRangeKernel(debugKernel, cl::NullRange, cl::NDRange(1000), cl::NullRange);
	queue.finish();
	queue.enqueueReadBuffer(countBuffer, CL_TRUE, 0, sizeof(counter), &counter);
	int cSize = sizeof(Instance);
	if(counter != cSize)
		std::cout << "Not matching!" << std::endl;
}


void OpenClRayTracer::fetchRayTracerResult() {
#ifndef RUN_ON_CPU
	//Give back ownership of OpenGL texture
	queue.enqueueReleaseGLObjects(&resultImages, NULL, NULL);
	queue.finish();//Make wait for it to be released
#else
	throw std::exception("Not yet implemented for calculation on cpu");
#endif // !1

	
	renderer.draw();
}

void OpenClRayTracer::initializeAdvancedRender() {

	rayBuffers.reserve(RAY_DEPTH);
	rayTreeBuffers.reserve(RAY_DEPTH);
	hitBuffers.reserve(RAY_DEPTH + 1);

	for (int i = 0; i < RAY_DEPTH; i++) {
		size_t raySize = sizeof(Ray);
		size_t rayBufferSize = sizeof(Ray) * width * height * (1 << (i));
		rayBuffers.push_back(cl::Buffer(context, CL_MEM_READ_WRITE, rayBufferSize));
		rayTreeBuffers.push_back(cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(RayTree) * width * height * (1 << i)));
		hitBuffers.push_back(cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Hit) * width * height * (1 << i)));
	}
}

void OpenClRayTracer::advancedRender(float16 matrix) {
	queue.finish();

	auto startTime = std::chrono::high_resolution_clock::now();

	cl_int rayCount = width * height;
	perspectiveRayGeneratorKernel.setArg(0, matrix);
	perspectiveRayGeneratorKernel.setArg(1, rayBuffers[0]);
	queue.enqueueNDRangeKernel(perspectiveRayGeneratorKernel, cl::NullRange, cl::NDRange(width, height));
	queue.finish();
	
	
	std::array<TimePoint, RAY_DEPTH> rayTracerStartTimes;
	rayTracerStartTimes[0] = std::chrono::high_resolution_clock::now();

	int instanceCount = objectInstances.size();
	rayTraceAdvancedKernel.setArg(0, sizeof(instanceCount), &instanceCount);
	rayTraceAdvancedKernel.setArg(1, transformedObjectBuffer);
	rayTraceAdvancedKernel.setArg(2, objectTypeIndexBuffer);
	rayTraceAdvancedKernel.setArg(3, transformedVertexBuffer);
	rayTraceAdvancedKernel.setArg(4, rayBuffers[0]);
	rayTraceAdvancedKernel.setArg(5, hitBuffers[0]);
	rayTraceAdvancedKernel.setArg(6, rayTreeBuffers[0]);
	queue.enqueueNDRangeKernel(rayTraceAdvancedKernel, cl::NullRange, cl::NDRange(width * height));
	queue.finish();



	cl::Buffer rayCountBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), &rayCount);
	rayGeneratorKernel.setArg(1, rayCountBuffer);


	std::vector<cl_int> rayCounts(RAY_DEPTH);


	std::array<TimePoint, RAY_DEPTH - 1> rayGeneratorStartTimes;

	int i;
	for (i = 0; i < RAY_DEPTH - 1; i++) {//Continue until maximum ray depth is reached or no more rays left to trace
		if (rayCount > (1 << i) * width * height)
			throw std::exception("Too large rayCount! Probably caused by some bug");			//Probably caused by some syncronization bug

		rayGeneratorStartTimes[i] = std::chrono::high_resolution_clock::now();

		rayCounts[i] = rayCount;

		cl_int startIndex;
		rayGeneratorKernel.setArg(0, hitBuffers[i]);
		rayGeneratorKernel.setArg(2, rayBuffers[i + 1]);
		rayGeneratorKernel.setArg(3, rayTreeBuffers[i]);
		startIndex = 0;																				//
		queue.enqueueWriteBuffer(rayCountBuffer, CL_TRUE, 0, sizeof(cl_int), &startIndex);			//Reset rayIndex to 0
		queue.enqueueNDRangeKernel(rayGeneratorKernel, cl::NullRange, cl::NDRange(rayCount));
		queue.finish();
		queue.enqueueReadBuffer(rayCountBuffer, CL_TRUE, 0, sizeof(cl_int), &rayCount);

		if (!rayCount) {//No rays left to trace(parents weren't refractive/reflective)
			break;
		}

		rayTracerStartTimes[i + 1] = std::chrono::high_resolution_clock::now();
		
		rayTraceAdvancedKernel.setArg(4, rayBuffers[i + 1]);
		rayTraceAdvancedKernel.setArg(5, hitBuffers[i + 1]);
		rayTraceAdvancedKernel.setArg(6, rayTreeBuffers[i + 1]);
		queue.enqueueNDRangeKernel(rayTraceAdvancedKernel, cl::NullRange, cl::NDRange(rayCount));
		queue.finish();

	}
	
	std::array<TimePoint, RAY_DEPTH - 1> treeTraverserStartTimes;

	for (; i > 0; i--) {
		rayCount = rayCounts[i - 1];
		//rayCounts.pop_back();
		
		treeTraverserStartTimes[RAY_DEPTH - i - 1] = std::chrono::high_resolution_clock::now();

		treeTraverserKernel.setArg(0, rayTreeBuffers[i - 1]);//[i - 1]);
		treeTraverserKernel.setArg(1, rayTreeBuffers[i]);
		queue.enqueueNDRangeKernel(treeTraverserKernel, cl::NullRange, cl::NDRange(rayCount));
		queue.finish();
	}

	auto colorToPixelStartTime = std::chrono::high_resolution_clock::now();

	colorToPixelKernel.setArg(0, rayTreeBuffers[0]);
	colorToPixelKernel.setArg(1, resultImages[0]);
	queue.enqueueNDRangeKernel(colorToPixelKernel, cl::NullRange, cl::NDRange(width, height));
	queue.finish();

	auto drawingStartTime = std::chrono::high_resolution_clock::now();
	fetchRayTracerResult();

	auto doneTime = std::chrono::high_resolution_clock::now();

	
	
	
	profileAdvancedRender(
		startTime,
		rayTracerStartTimes,
		rayGeneratorStartTimes,
		treeTraverserStartTimes,
		colorToPixelStartTime,
		drawingStartTime,
		doneTime
	);
}

void OpenClRayTracer::profileAdvancedRender(
	TimePoint startTime,
	std::array<TimePoint, RAY_DEPTH> rayTracerStartTimes,
	std::array<TimePoint, RAY_DEPTH - 1> rayGeneratorStartTimes,
	std::array<TimePoint, RAY_DEPTH - 1> treeTraverserStartTimes,
	TimePoint colorToPixelStartTime,
	TimePoint drawingStartTime,
	TimePoint doneTime
) {
	static auto lastTimeOfReport = std::chrono::high_resolution_clock::now();
	auto deltaTime = doneTime - lastTimeOfReport;
	if (deltaTime < std::chrono::seconds(5))
		return;

	lastTimeOfReport += deltaTime;
	std::cout << std::fixed << std::setprecision(2);
	std::cout << "----Total Time: " << durationToMs(doneTime - startTime) << "ms----" << std::endl;
	std::cout << "--------" << std::endl;
	
	std::cout << "PerspectiveRayGenerator: " << durationToMs(rayTracerStartTimes[0] - startTime) << "ms----" << std::endl;
	for (int i = 0; i < RAY_DEPTH - 2; i++) {
		std::cout << "RayGenerator run(" << (i + 1) << "): " << durationToMs(rayTracerStartTimes[i + 1] - rayGeneratorStartTimes[i]) << "ms----" << std::endl;
	}
	std::cout << "RayGenerator run(" << (RAY_DEPTH - 1) << "): " << durationToMs(rayTracerStartTimes[RAY_DEPTH - 1] - rayGeneratorStartTimes[RAY_DEPTH - 2]) << "ms----" << std::endl;
	std::cout << "--------" << std::endl;
	
	std::cout << "RayTracer run(0): " << durationToMs(rayGeneratorStartTimes[0] - rayTracerStartTimes[0]) << "ms----" << std::endl;
	for (int i = 0; i < RAY_DEPTH - 2; i++) {
		std::cout << "RayTracer run(" << (i + 1) << "): " << durationToMs(rayGeneratorStartTimes[i + 1] - rayTracerStartTimes[i + 1]) << "ms----" << std::endl;
	}
	std::cout << "RayTracer run(" << (RAY_DEPTH - 1) << "): " << durationToMs(treeTraverserStartTimes[0] - rayTracerStartTimes[RAY_DEPTH - 1]) << "ms----" << std::endl;
	std::cout << "--------" << std::endl;

	for (int i = 1; i < RAY_DEPTH - 1; i++) {
		std::cout << "TreeTraverse run(" << (i - 1) << "): " << durationToMs(treeTraverserStartTimes[i] - treeTraverserStartTimes[i - 1]) << "ms----" << std::endl;
	}
	std::cout << "TreeTraverse run(" << (RAY_DEPTH - 2) << "): " << durationToMs(colorToPixelStartTime - treeTraverserStartTimes[RAY_DEPTH - 2]) << "ms----" << std::endl;
	std::cout << "--------" << std::endl;

	std::cout << "ColorToPixel: " << durationToMs(drawingStartTime - colorToPixelStartTime) << "ms----" << std::endl;
	std::cout << "Drawing: " << durationToMs(doneTime - drawingStartTime) << "ms----" << std::endl;
	for(int i = 0; i < 10; i++)
		std::cout << std::endl;
}

double OpenClRayTracer::durationToMs(Duration duration) {
	return std::chrono::duration<double, std::milli>(duration).count();
}


void OpenClRayTracer::resizeCallback(GLFWwindow* window, int width, int height) {
	this->width = width;
	this->height = height;

#ifdef ADVANCED_RENDERER
	throw "Resize Not implemented!";
#endif // ADVANCED_RENDERER


	renderer.resizeCallback(window, width, height);

	cl_int status;


#ifndef RUN_ON_CPU
	resultImages[0] = cl::ImageGL(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, openGlTextureID, &status);
	if (status != CL_SUCCESS) {
		std::cout << "Failed to create OpenCL image from OpenGL texture" << std::endl;
		exit(1);
	}
#else
	resultImages[0] = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float4) * width * height, &status);
	if (status != CL_SUCCESS) {
		std::cout << "Failed to create OpenCL image from OpenGL texture" << std::endl;
		exit(1);
	}
#endif // !RUN_ON_CPU
}