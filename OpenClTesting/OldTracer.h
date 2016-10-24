#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <CL/cl.hpp>

using namespace glm;

class OldTracer
{
private:

	GLuint* GLtexture;
	std::vector<cl::Memory> Screen;//Result of the ray tracer to be sent to the screen

								   //List of OpenCL source files
	cl::Program::Sources sources;

	cl::Program program;
	cl::Kernel kernel_RayTracer;

	//Reference to OpenGL texture
	cl::CommandQueue Queue;

	//

	cl::Buffer NumVerticesBuffer;
	cl::Buffer VertexBuffer;
	cl::Buffer VertexNormalBuffer;

	cl::Buffer NumIndicesBuffer;
	cl::Buffer IndexBuffer;

	cl::Buffer numObjBuffer;

	cl::Buffer ObjectsVertexBuffer;	// Start vertex for object
	cl::Buffer ObjectsIndexBuffer;	//Start index for object

	cl::Buffer ObjectsCenterPosBuffer;
	cl::Buffer ObjectsRadiusBuffer;
	cl::Buffer ObjectsColorBuffer;

	cl::Buffer NumLightSourcesBuffer;
	cl::Buffer LightSourcesBuffer;
	//

	cl::Memory mem;

	int PrefferedDeviceType = CL_DEVICE_TYPE_GPU;

public:
	int Width, Height;

	OldTracer();
	OldTracer(int width, int height);
	~OldTracer();
	bool Init(const int MAX_VERTICES, const int MAX_INDICES, const int MAX_OBJECTS, GLuint & gltexture);

	void Run(int numvertices, vec3 vertices[], vec3 verticesNormals[], int numIndices, int indices[], int numObjects, int objectsVertices[], int objectsIndices[], vec3 objectsCenterPos[], float objectsRadius[], vec3 objectsColor[], int numLightSources, Light lightSources[]);

	std::string LoadKernelFromFile(std::string fileName);
	bool convertToString(std::string filename, std::string & s);
	const std::string getErrorString(cl_int error);
};