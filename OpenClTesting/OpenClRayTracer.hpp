#pragma once

#include "ArraySlice.hpp"
#include "Containers.hpp"

#include <vector>
#include "CL\cl.hpp"
#include "GL\glew.h"
#include "GLFW\glfw3.h"

class OpenClRayTracer
{
public:
	OpenClRayTracer(int width, int height, cl::Context context, cl::Device device, int maxObjectCount, int maxTriangleCount, int maxVertexCount, GLuint openGlTextureID);
	~OpenClRayTracer();
	Object push_back(std::vector<TriangleIndices>& trianglesIndices, std::vector<Vertex>& vertices);
	ArraySlice<TriangleIndices> getTriangles(Object object);
	ArraySlice<Vertex> getVertices(Object object);

	AABB generateAABB(Vertex* vertices, int vertexCount);
	inline AABB OpenClRayTracer::generateAABB(std::vector<Vertex> vector);

	void writeToBuffers();

	void resize();
	void resize(int maxObjectCount, int maxTriangleCount, int maxVertexCount);


	void computeOnCPU();
	cl::Event computeNonBlocking(float16 matrix);
	void compute(float16 matrix);
	void fetchResult();
	cl::Event debug(float16 matrix);

private:
	void initialize(cl::Context context, cl::Device device);
	void resizeArrays(int maxObjectCount, int maxTriangleCount, int maxVertexCount);
	void resizeBuffers(int maxObjectCount, int maxTriangleCount, int maxVertexCount);

	std::vector<Object> objects;
	std::vector<TriangleIndices> triangles;
	std::vector<Vertex> vertices;
	std::vector<float4> result;

	cl::Kernel computeKernel;
	cl::Context context;
	cl::CommandQueue queue;

	cl::Buffer objectBuffer;
	cl::Buffer triangleBuffer;
	cl::Buffer vertexBuffer;
	std::vector<cl::Memory> resultImages;// Has to be vector even though there is only one element due to queue.enqueueAcquireGLObjects
	GLuint openGlTextureID;

	int width;
	int height;
	
};

