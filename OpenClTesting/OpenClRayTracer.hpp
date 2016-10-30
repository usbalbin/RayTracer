#pragma once

#include "ArraySlice.hpp"
#include "Containers.hpp"
#include "OpenGlShaders.hpp"

#include <vector>
#include "CL\cl.hpp"
#include "GL\glew.h"
#include "GLFW\glfw3.h"

class OpenClRayTracer
{
public:
	OpenClRayTracer(int width, int height, int maxInstanceCount, int maxTotalVertexCount);
	~OpenClRayTracer();

	void clear();
	void push_back(Instance instance);
	Instance pop_instance();

	InstanceBuilder push_backObjectType(std::vector<TriangleIndices>& objectTypeIndices, std::vector<Vertex>& objectTypeVertices);

	
	//ArraySlice<TriangleIndices> getTriangles(Object object);	// Doing stuff to thise object type will alter every instance of this object type once the buffers are updated
	//ArraySlice<Vertex> getVertices(Object object);			// Not yet implemented

	void writeToObjectTypeBuffers();

	void writeToInstanceBuffer();

	

	void autoResize();
	void autoResizeObjectTypes();
	void reserve(int maxInstanceCount, int maxTotalVertexCount);

	void computeOnCPU();
	cl::Event vertexShaderNonBlocking();
	cl::Event aabbNonBlocking();
	cl::Event prepRayTraceNonBlocking();
	cl::Event rayTraceNonBlocking(float16 matrix);
	void sizeofDebug();
	void rayTrace(float16 matrix);
	void fetchRayTracerResult();

	void initializeAdvancedRender();

	GLFWwindow* getWindow() {return renderer.getWindow();}
	void advancedRender(float16 matrix);

	void resizeCallback(GLFWwindow * window, int width, int height);

private:
	void initialize();
	void reserveArrays(int maxInstanceCount);
	void reserveObjectTypeBuffers(int maxObjectTypeCount, int maxObjectTypeTriangleCount, int maxObjectTypeVertexCount);
	void reserveBuffers(int maxObjectCount, int maxVertexCount);

	std::vector<Object> objectTypes;
	std::vector<TriangleIndices> objectTypeIndices;
	std::vector<Vertex> objectTypeVertices;
	std::vector<Instance> objectInstances;

	unsigned transformedVertexCount = 0;

	cl::Kernel vertexShaderKernel;
	cl::Kernel aabbKernel;
	cl::Kernel rayTraceKernel;
	cl::Kernel iterativeRayTracerKernel;
	cl::Kernel sizeofKernel;

	cl::Kernel perspectiveRayGeneratorKernel;
	cl::Kernel rayTraceAdvancedKernel;
	cl::Kernel rayGeneratorKernel;
	cl::Kernel treeTraverserKernel;
	cl::Kernel colorToPixelKernel;

	cl::Context context;
	cl::CommandQueue queue;


	cl::Buffer objectTypeBuffer;
	cl::Buffer objectTypeIndexBuffer;
	cl::Buffer objectTypeVertexBuffer;

	cl::Buffer objectInstanceBuffer;
	cl::Buffer transformedObjectBuffer;
	cl::Buffer transformedVertexBuffer;

	std::vector<cl::Buffer> rayBuffers;
	std::vector<cl::Buffer> rayTreeBuffers;
	std::vector<cl::Buffer> hitBuffers;


	std::vector<cl::Memory> resultImages;// Has to be vector even though there is only one element due to queue.enqueueAcquireGLObjects
	
	OpenGlShaders renderer;
	GLuint openGlTextureID;

	int width;
	int height;

	
};

