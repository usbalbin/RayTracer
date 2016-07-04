#include "stdafx.h"

#include "OpenClRayTracer.hpp"
#include "DebugRayTracer.hpp"
#include "Utils.hpp"
#include "glm\glm.hpp"

#include "GLFW\glfw3.h"

#include <iostream>


OpenClRayTracer::OpenClRayTracer(int width, int height, cl::Context context, cl::Device device, int maxObjectCount, int maxTriangleCount, int maxVertexCount, GLuint openGlTextureID) {
	this->width = width;
	this->height = height;
	this->openGlTextureID = openGlTextureID;
	initialize(context, device);
	resize(maxObjectCount, maxTriangleCount, maxVertexCount);
}

OpenClRayTracer::~OpenClRayTracer()
{
}

void OpenClRayTracer::initialize(cl::Context context, cl::Device device) {
	this->context = context;


	cl::Program::Sources sources;

	std::string sourceCode = readFileToString("kernels/rayTracerMain.cl");
	//std::string sourceCode = readFileToString("kernels/testDemSaiZze.cl");


	

	sources.push_back({ sourceCode.c_str(), sourceCode.length() });

	cl::Program program(context, sources);


	if (program.build({ device }) != CL_SUCCESS) {
		std::cout << "Failed to compile: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
		system("pause");
		exit(1);
	}

	queue = cl::CommandQueue(context, device, NULL, NULL);

	computeKernel = cl::Kernel(program, "computeKernel");
}

void OpenClRayTracer::resize() {
	resizeArrays(vertices.size(), triangles.size(), vertices.size());
	resizeBuffers(vertices.size(), triangles.size(), vertices.size());
}

void OpenClRayTracer::resize(int maxObjectCount, int maxTriangleCount, int maxVertexCount) {
	resizeArrays(maxObjectCount, maxTriangleCount, maxVertexCount);
	resizeBuffers(maxObjectCount, maxTriangleCount, maxVertexCount);
}

void OpenClRayTracer::resizeArrays(int maxObjectCount, int maxTriangleCount, int maxVertexCount) {
	this->objects.reserve(maxObjectCount);
	this->triangles.reserve(maxTriangleCount);
	this->vertices.reserve(maxVertexCount);
}

void OpenClRayTracer::resizeBuffers(int maxObjectCount, int maxTriangleCount, int maxVertexCount) {
	objectBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Object) * maxObjectCount);
	triangleBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(TriangleIndices) * maxTriangleCount);
	vertexBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Vertex) * maxVertexCount);

	GLint status;
	resultImages.push_back(cl::ImageGL(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, openGlTextureID, &status));
	if (status != CL_SUCCESS) {
		std::cout << "Failed to create OpenCL image from OpenGL texture" << std::endl;
		exit(1);
	}

}

Object OpenClRayTracer::push_back(std::vector<TriangleIndices>& trianglesIndices, std::vector<Vertex>& vertices) {
	Object object;
	object.boundingBox = generateAABB(vertices);
	object.startTriangle = this->triangles.size();
	object.startVertex = this->vertices.size();
	object.numTriangles = trianglesIndices.size();
	object.numVertices = vertices.size();


	this->objects.push_back(object);
	this->triangles.insert(std::end(this->triangles), std::begin(trianglesIndices), std::end(trianglesIndices));
	this->vertices.insert(std::end(this->vertices), std::begin(vertices), std::end(vertices));

	return object;
}

ArraySlice<TriangleIndices> OpenClRayTracer::getTriangles(Object object)
{
	return ArraySlice<TriangleIndices>(triangles, object.startTriangle, object.numTriangles);
}

ArraySlice<Vertex> OpenClRayTracer::getVertices(Object object)
{
	return ArraySlice<Vertex>(vertices, object.startVertex, object.numVertices);
}

inline AABB OpenClRayTracer::generateAABB(std::vector<Vertex> vector) {
	return generateAABB(vector.data(), vector.size());
}

AABB OpenClRayTracer::generateAABB(Vertex* vertices, int vertexCount) {
	AABB res;
	res.max = res.min = vertices[0].position;

	for (int i = 1; i < vertexCount; i++) {
		res.min = glm::min(vertices[i].position, res.min);
		res.max = glm::max(vertices[i].position, res.max);
	}

	return res;
}

void OpenClRayTracer::writeToBuffers() {
	if (queue.enqueueWriteBuffer(objectBuffer, CL_TRUE, 0, sizeof(Object) * objects.size(), objects.data()) != CL_SUCCESS) {
		std::cout << "Failed to write to buffer" << std::endl;
		exit(1);
	}

	if (queue.enqueueWriteBuffer(triangleBuffer, CL_TRUE, 0, sizeof(TriangleIndices) * triangles.size(), triangles.data()) != CL_SUCCESS) {
		std::cout << "Failed to write to buffer" << std::endl;
		exit(1);
	}

	if (queue.enqueueWriteBuffer(vertexBuffer, CL_TRUE, 0, sizeof(Vertex) * vertices.size(), vertices.data()) != CL_SUCCESS) {
		std::cout << "Failed to write to buffer" << std::endl;
		exit(1);
	}

	int objectCount = objects.size();
	if (computeKernel.setArg(0, sizeof(objectCount), &objectCount) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}

	if (computeKernel.setArg(2, objectBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	if (computeKernel.setArg(3, triangleBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	if (computeKernel.setArg(4, vertexBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}

}

void OpenClRayTracer::compute(float16 matrix) {
	computeNonBlocking(matrix).wait();
	fetchResult();
}


void OpenClRayTracer::computeOnCPU()
{
	result = cpuRayTrace(width, height, objects, triangles, vertices);
}

cl::Event OpenClRayTracer::computeNonBlocking(float16 matrix) {
	
	
	//Make sure OpenGL is done working
	glFinish();

	//Take ownership of OpenGL texture
	if (queue.enqueueAcquireGLObjects(&resultImages, NULL, NULL) != CL_SUCCESS) {
		std::cout << "Failed to acquire result Texture from OpenGL" << std::endl;
		exit(1);
	}
	queue.finish();//Make sure OpenCL has grabbed the texture from GL(probably not needed)
	
	
	if (computeKernel.setArg(5, resultImages[0]) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}

	if (computeKernel.setArg(1, sizeof(float16), &matrix) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	queue.finish();

	cl::Event event;

	queue.enqueueNDRangeKernel(computeKernel, cl::NullRange, cl::NDRange(width, height), cl::NullRange, 0, &event);


	return event;
}




void OpenClRayTracer::fetchResult() {

	//Give back ownership of OpenGL texture
	queue.enqueueReleaseGLObjects(&resultImages, NULL, NULL);
	queue.finish();//Make wait for it to be released
}

cl::Event OpenClRayTracer::debug(float16 matrix) {

	if (queue.enqueueWriteBuffer(objectBuffer, CL_TRUE, 0, sizeof(Object) * objects.size(), objects.data()) != CL_SUCCESS) {
		std::cout << "Failed to write to buffer" << std::endl;
		exit(1);
	}

	if (queue.enqueueWriteBuffer(triangleBuffer, CL_TRUE, 0, sizeof(TriangleIndices) * triangles.size(), triangles.data()) != CL_SUCCESS) {
		std::cout << "Failed to write to buffer" << std::endl;
		exit(1);
	}

	if (queue.enqueueWriteBuffer(vertexBuffer, CL_TRUE, 0, sizeof(Vertex) * vertices.size(), vertices.data()) != CL_SUCCESS) {
		std::cout << "Failed to write to buffer" << std::endl;
		exit(1);
	}

	//Make sure OpenGL is done working
	glFinish();

	//Take ownership of OpenGL texture
	if (queue.enqueueAcquireGLObjects(&resultImages, NULL, NULL) != CL_SUCCESS) {
		std::cout << "Failed to acquire result Texture from OpenGL" << std::endl;
		exit(1);
	}
	queue.finish();//Make sure OpenCL has grabbed the texture from GL(probably not needed)

	int objectCount = objects.size();
	if (computeKernel.setArg(0, sizeof(objectCount), &objectCount) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}

	if (computeKernel.setArg(1, sizeof(float16), &matrix) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}

	if (computeKernel.setArg(2, objectBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	if (computeKernel.setArg(3, triangleBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	if (computeKernel.setArg(4, vertexBuffer) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}
	if (computeKernel.setArg(5, resultImages[0]) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}

	
	int openClOut = 0;
	cl::Buffer sizeBuff(context, CL_MEM_WRITE_ONLY, sizeof(openClOut));

	if (computeKernel.setArg(6, sizeBuff) != CL_SUCCESS) {
		std::cout << "Failed to set argument" << std::endl;
		exit(1);
	}


	queue.finish();

	cl::Event event;

	queue.enqueueNDRangeKernel(computeKernel, cl::NullRange, cl::NDRange(1), cl::NullRange, 0, &event);



	
	event.wait();
	queue.enqueueReadBuffer(sizeBuff, CL_TRUE, 0, sizeof(openClOut), &openClOut);
	queue.finish();

	int float3Size = sizeof(float3);		//12 - 16
	int aaBBSize = sizeof(AABB);			//32 - 32
	int objSize = sizeof(Object);			//48 - 48
	int triSize = sizeof(Triangle);			//48 - 48
	int triISize = sizeof(TriangleIndices);	//12 - 12

	int vertexSize = sizeof(Vertex);
	float last = vertices[2].position.z;
	
	return event;
}

