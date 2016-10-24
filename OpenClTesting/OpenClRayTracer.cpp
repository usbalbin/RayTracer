#include "stdafx.h"

#include "OpenClRayTracer.hpp"
#include "OpenClContexts.hpp"
#include "DebugRayTracer.hpp"
#include "Utils.hpp"

#include "GLFW\glfw3.h"

#include <iostream>
#include <stack>

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
	this->openGlTextureID = renderer.setupInteropScreen();
	initialize();
	reserve(maxInstanceCount, maxTotalVertexCount);
}

OpenClRayTracer::~OpenClRayTracer()
{
}

void OpenClRayTracer::initialize() {
	OpenClContexts openClContexts;
	openClContexts.initializeInteropGpu();

	cl::Device device = openClContexts.getGpuDevice(0);
	this->context = openClContexts.getGpuContext(0);


	cl::Program::Sources sources;

	std::string vertexShaderSource = readFileToString("kernels/vertexShader.cl");
	std::string aabbSource = readFileToString("kernels/aabb.cl");
	//std::string rayTracerSource = readFileToString("kernels/oldKernels/rayTracerMain.cl");
	//std::string iterativeRayTracerSource = readFileToString("kernels/iterativeDepth.cl");
	//std::string sizeofSource = readFileToString("kernels/sizeof.cl");

	std::string perspectiveRayGeneratorSource = readFileToString("kernels/newKernels/1_perspectiveRayGenerator.cl");
	std::string rayTraceAdvancedSource = readFileToString("kernels/newKernels/2A_rayTracer.cl");
	std::string rayGeneratorSource = readFileToString("kernels/newKernels/2B_rayGenerator.cl");
	std::string treeTraverserSource = readFileToString("kernels/newKernels/3_treeTraverser.cl");
	std::string colorToPixelSource = readFileToString("kernels/newKernels/4_colorToPixel.cl");




	sources.push_back({ vertexShaderSource.c_str(), vertexShaderSource.length() });
	sources.push_back({ aabbSource.c_str(), aabbSource.length() });
	//sources.push_back({ rayTracerSource.c_str(), rayTracerSource.length() });
	//sources.push_back({ iterativeRayTracerSource.c_str(), iterativeRayTracerSource.length() });
	//sources.push_back({ sizeofSource.c_str(), sizeofSource.length() });

	sources.push_back({ perspectiveRayGeneratorSource.c_str(), perspectiveRayGeneratorSource.length() });
	sources.push_back({ rayTraceAdvancedSource.c_str(), rayTraceAdvancedSource.length() });
	sources.push_back({ rayGeneratorSource.c_str(), rayGeneratorSource.length() });
	sources.push_back({ treeTraverserSource.c_str(), treeTraverserSource.length() });
	sources.push_back({ colorToPixelSource.c_str(), colorToPixelSource.length() });


	cl::Program program(context, sources);



	std::cout << "---------------- Compilation status ----------------" << std::endl;
	std::string compileMessage;
	char programPathBuffer[256];
	getcwd(programPathBuffer, 256);
	std::string programPath = programPathBuffer;
	std::string stuff = device.getInfo<CL_DEVICE_OPENCL_C_VERSION>();

	std::cout << "Path: \"" << programPath << "\"" << std::endl;

















	// KOLLA PÅ						-CL-STD=CL2.0














	std::string extraOptions = "-cl-std=CL2.0";// "-cl-std=c++";// "-cl-std=CL2.0";// "-cl-unsafe-math-optimizations -cl-fast-relaxed-math";
	std::string compilerFlags = "-I " + programPath + " " + extraOptions;
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
	//rayTraceKernel = cl::Kernel(program, "rayTracer", &status);
	//iterativeRayTracerKernel = cl::Kernel(program, "iterative", &status);
	//sizeofKernel = cl::Kernel(program, "debug", &status);

	perspectiveRayGeneratorKernel = cl::Kernel(program, "perspectiveRayGenerator", &status);
	rayTraceAdvancedKernel = cl::Kernel(program, "rayTraceAdvanced", &status);
	rayGeneratorKernel = cl::Kernel(program, "rayGenerator", &status);
	treeTraverserKernel = cl::Kernel(program, "treeTraverser", &status);
	colorToPixelKernel = cl::Kernel(program, "colorToPixel", &status);

	if (status != CL_SUCCESS) {
		std::cout << "Failed to create kernels" << std::endl;
		exit(1);
	}
	
	resultImages[0] = cl::ImageGL(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, openGlTextureID, &status);
	if (status != CL_SUCCESS) {
		std::cout << "Failed to create OpenCL image from OpenGL texture" << std::endl;
		exit(1);
	}
}

void OpenClRayTracer::autoResize() {
	reserveArrays(objectInstances.size());
	reserveBuffers(objectInstances.size(), transformedVertexCount);
}

void OpenClRayTracer::autoResizeObjectTypes() {
	reserveObjectTypeBuffers(objectTypes.size(), objectTypeIndices.size(), objectTypeVertices.size());
}

void OpenClRayTracer::reserve(int maxInstanceCount, int maxTotalVertexCount){
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
	rayTraceNonBlocking(matrix).wait();
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

	
	//debug
	event.wait();

	std::vector<Object> objs(objectInstances.size());
	queue.enqueueReadBuffer(transformedObjectBuffer, CL_TRUE, 0, sizeof(Object) * objs.size(), objs.data());
	
	std::vector<Instance> insts(objectInstances.size());
	queue.enqueueReadBuffer(objectInstanceBuffer, CL_TRUE, 0, sizeof(Instance) * insts.size(), insts.data());
	
	queue.finish();
	
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

	/*
	//debug
	event.wait();

	std::vector<Object> objs;
	objs.resize(objectInstances.size());
	queue.enqueueReadBuffer(transformedObjectBuffer, CL_TRUE, 0, sizeof(Object) * objs.size(), objs.data());
	queue.finish();
	*/
	

	return event;

}

//Give me a better name
cl::Event OpenClRayTracer::prepRayTraceNonBlocking() {
	vertexShaderNonBlocking().wait();		//WARNING CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE can not be set for cammand queue for this to work!!!
	return aabbNonBlocking();
}

cl::Event OpenClRayTracer::rayTraceNonBlocking(float16 matrix) {
	
	
	//Make sure OpenGL is done working
	glFinish();

	//Take ownership of OpenGL texture
	if (queue.enqueueAcquireGLObjects(&resultImages, NULL, NULL) != CL_SUCCESS) {
		std::cout << "Failed to acquire result Texture from OpenGL" << std::endl;
		exit(1);
	}
	queue.finish();//Make sure OpenCL has grabbed the texture from GL(probably not needed)
	

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
	
	/*
	int objectCount = objectTypes.size();
	rayTraceKernel.setArg(0, sizeof(objectCount), &objectCount);
	rayTraceKernel.setArg(1, sizeof(float16), &matrix);
	rayTraceKernel.setArg(2, objectTypeBuffer);
	rayTraceKernel.setArg(3, objectTypeIndexBuffer);
	rayTraceKernel.setArg(4, objectTypeVertexBuffer);
	rayTraceKernel.setArg(5, resultImages[0]);
	*/
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



void OpenClRayTracer::sizeofDebug() {
	int size;
	cl::Buffer sizeBuffer(context, CL_MEM_READ_WRITE, sizeof(size));


	if (sizeofKernel.setArg(0, sizeBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}

	cl::Event event;
	queue.enqueueNDRangeKernel(sizeofKernel, cl::NullRange, cl::NDRange(1), cl::NullRange, 0, &event);
	event.wait();

	queue.enqueueReadBuffer(sizeBuffer, CL_TRUE, 0, sizeof(size), &size);
	int cSize = sizeof(Instance);
	if(size != cSize)
		std::cout << "Not matching!" << std::endl;
}


void OpenClRayTracer::fetchRayTracerResult() {
	//Give back ownership of OpenGL texture
	queue.enqueueReleaseGLObjects(&resultImages, NULL, NULL);
	queue.finish();//Make wait for it to be released
	renderer.draw();
}

void OpenClRayTracer::initializeAdvancedRender() {
	const int max = 3;

	rayBuffers.resize(max + 1);
	rayTreeBuffers.resize(max);
	hitBuffers.resize(max);

	for (int i = 0; i < max; i++) {
		rayBuffers.push_back(cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Ray) * width * height * (1 << i)));
		rayTreeBuffers.push_back(cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(RayTree) * width * height * (1 << i)));
		hitBuffers.push_back(cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Hit) * width * height * (1 << i)));
	}
	rayBuffers.push_back(cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Ray) * width * height * (1 << max)));
}

void OpenClRayTracer::advancedRender(float16 matrix) {
	const int max = 3;
	int rayCount = width * height;
	perspectiveRayGeneratorKernel.setArg(0, matrix);
	perspectiveRayGeneratorKernel.setArg(1, rayBuffers[0]);
	queue.enqueueNDRangeKernel(perspectiveRayGeneratorKernel, cl::NullRange, cl::NDRange(width, height));

	cl::Buffer rayCountBuffer(context, CL_MEM_READ_WRITE, sizeof(rayCount));
	rayGeneratorKernel.setArg(1, rayCountBuffer);


	std::stack<int> rayCounts;
	rayCounts.push(rayCount);

	int instanceCount = objectInstances.size();
	rayTraceAdvancedKernel.setArg(0, sizeof(instanceCount), &instanceCount);
	rayTraceAdvancedKernel.setArg(1, transformedObjectBuffer);
	rayTraceAdvancedKernel.setArg(2, objectTypeIndexBuffer);
	rayTraceAdvancedKernel.setArg(3, transformedVertexBuffer);
	rayTraceAdvancedKernel.setArg(6, rayCountBuffer);


	int i = 0;
	for (i = 0; i < max; i++) {
		rayTraceAdvancedKernel.setArg(4, rayBuffers[i]);
		rayTraceAdvancedKernel.setArg(5, hitBuffers[i]);
		queue.enqueueNDRangeKernel(rayTraceAdvancedKernel, cl::NullRange, cl::NDRange(rayCount));
		queue.enqueueReadBuffer(rayCountBuffer, CL_TRUE, 0, sizeof(rayCount), &rayCount);	//Remove this line


		rayGeneratorKernel.setArg(0, hitBuffers[i]);
		rayGeneratorKernel.setArg(2, rayBuffers[i]);
		rayGeneratorKernel.setArg(3, rayTreeBuffers[i]);
		queue.enqueueNDRangeKernel(rayGeneratorKernel, cl::NullRange, cl::NDRange(rayCount));
		queue.enqueueReadBuffer(rayCountBuffer, CL_TRUE, 0, sizeof(rayCount), &rayCount);
		rayCounts.push(rayCount);
	}
	rayCounts.pop(); //Remove last element, last set of rays wont be used 

	for (i--; i > 0; i--) {
		rayCount = rayCounts.top();
		rayCounts.pop();

		treeTraverserKernel.setArg(0, rayTreeBuffers[i - 1]);
		treeTraverserKernel.setArg(1, rayTreeBuffers[i]);
		queue.enqueueNDRangeKernel(treeTraverserKernel, cl::NullRange, cl::NDRange(rayCount));
	}

	colorToPixelKernel.setArg(0, rayTreeBuffers[0]);
	colorToPixelKernel.setArg(1, resultImages[0]);
	queue.enqueueNDRangeKernel(colorToPixelKernel, cl::NullRange, cl::NDRange(width * height));
}


void OpenClRayTracer::resizeCallback(GLFWwindow* window, int width, int height) {
	this->width = width;
	this->height = height;
	renderer.resizeCallback(window, width, height);

	cl_int status;
	resultImages[0] = cl::ImageGL(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, openGlTextureID, &status);
	if (status != CL_SUCCESS) {
		std::cout << "Failed to create OpenCL image from OpenGL texture" << std::endl;
		exit(1);
	}
}