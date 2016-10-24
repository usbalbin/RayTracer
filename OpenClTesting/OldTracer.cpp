
#include "OldTracer.h"
#include <fstream>
#include <streambuf>
#include <glm/glm.hpp>


OldTracer::OldTracer(int width, int height) : Width(width), Height(height)
{

}

OldTracer::~OldTracer()
{
}

bool OldTracer::Init(const int MAX_VERTICES, const int MAX_INDICES, const int MAX_OBJECTS, GLuint& gltexture)
{
	int err = CL_SUCCESS;

	//Reference to OpenGL texture
	GLtexture = &gltexture;


	//get all platforms (drivers)
	std::vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);
	if (all_platforms.size() == 0) {
		std::cout << " No platforms found. Check OpenCL installation!\n";
		return false;
	}
	cl::Platform default_platform = all_platforms[0];
	std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

	//get default device of the default platform
	std::vector<cl::Device> all_devices;
	default_platform.getDevices(PrefferedDeviceType, &all_devices);
	if (all_devices.size() == 0) {
		std::cout << " No devices found. Check OpenCL installation!\n";
		return false;
	}
	cl::Device default_device = all_devices[0];
	std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";


	cl_context_properties context_properties[] =
	{
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
		CL_CONTEXT_PLATFORM, (cl_context_properties)default_platform(),
		NULL
	};

	cl::Context context({ default_device }, context_properties, NULL, NULL, &err);
	if (err != CL_SUCCESS)
	{
		std::cout << " Error binding texture\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}


	std::string kernel_code;/* =
							"\#pragma OPENCL EXTENSION cl_amd_printf : enable\n"
							"\n"
							"const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_FILTER_NEAREST;\n"
							"	\n"
							"	void kernel RayTracer(\n"
							"	global const int* numVertices,\n"
							"	global const float3* Vertices, \n"
							"	global const float3* VerticesNormals, \n"
							"	global const int* numIndices, \n"
							"	global const int* Indices, \n"
							"	global const int* numObjects, \n"
							"	global const int* ObjectsIndice, \n"
							"	global const int* ObjectsVertex, \n"
							"	global const float3* ObjectsCenterPos, \n"
							"	global const float* ObjectsRadius, \n"
							"	global const float3* ObjectsColor, \n"
							"	write_only image2d_t output\n"
							"	) \n"
							"{\n"
							"	int2 pos = (int2)(get_global_id(0), get_global_id(1)); \n"
							"	\n"
							"	float4 color = (float4)(pos.x / 1024.0f, pos.y / 768.0f, 0.0f, 1.0f);\n"
							"	//float4 color = (float4)(1.0f, 1.0f, 0.0f, 1.0f); \n"
							"	\n"
							"	write_imagef(output, pos, color.xyzw);\n"
							"	//printf(\"%f %f %f %f\          \\n\", color.x, color.y, color.z, color.w); \n"
							"} \n";*/
	kernel_code = LoadKernelFromFile("RayTracer.cl");

	sources.push_back({ kernel_code.c_str(),kernel_code.length() });


	program = cl::Program(context, sources);
	if (program.build({ default_device }, "-w") != CL_SUCCESS) {
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
		return false;
	}

#pragma region "BindBuffers"
	NumVerticesBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int), &err);
	if (err != CL_SUCCESS)
	{
		std::cout << " Error binding texture\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}
	// create buffers on the device
	VertexBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(vec3) * MAX_VERTICES, &err);
	if (err != CL_SUCCESS)
	{
		std::cout << " Error binding texture\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}
	VertexNormalBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(vec3) * MAX_VERTICES, &err);
	if (err != CL_SUCCESS)
	{
		std::cout << " Error binding texture\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}

	NumIndicesBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int), &err);
	if (err != CL_SUCCESS)
	{
		std::cout << " Error binding texture\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}
	IndexBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int) * MAX_INDICES, &err);
	if (err != CL_SUCCESS)
	{
		std::cout << " Error binding texture\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}


	numObjBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int) * 1, &err);
	if (err != CL_SUCCESS)
	{
		std::cout << " Error binding texture\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}

	ObjectsVertexBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int) * (MAX_OBJECTS + 1), &err);	// Start vertex for object
	if (err != CL_SUCCESS)
	{
		std::cout << " Error binding texture\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}
	ObjectsIndexBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int) * (MAX_OBJECTS + 1), &err);	//Start index for object
	if (err != CL_SUCCESS)
	{
		std::cout << " Error binding texture\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}

	ObjectsCenterPosBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(vec3) * (MAX_OBJECTS + 1), &err);
	if (err != CL_SUCCESS)
	{
		std::cout << " Error binding texture\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}

	ObjectsRadiusBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float) * (MAX_OBJECTS + 1), &err);
	if (err != CL_SUCCESS)
	{
		std::cout << " Error binding texture\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}
	ObjectsColorBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(vec3) * (MAX_OBJECTS + 1), &err);
	if (err != CL_SUCCESS)
	{
		std::cout << " Error binding texture\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}
#pragma endregion "BindBuffers"
	//create queue to which we will push commands for the device.
	Queue = cl::CommandQueue(context, default_device, 0, &err);
	if (err != CL_SUCCESS)
	{
		std::cout << " Error binding texture\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}

	//Binding the result of the ray tracer to OpenGL texture
	Screen.push_back(cl::ImageGL(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, gltexture, &err));
	if (err != CL_SUCCESS)
	{
		std::cout << " Error binding texture\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}

	kernel_RayTracer = cl::Kernel(program, "RayTracer", &err);
	if (err != CL_SUCCESS)
	{
		std::cout << " Error creating kernel\n";
		std::cerr << getErrorString(err) << std::endl;
		return false;
	}


	return true;
}

void OldTracer::Run(int numvertices, vec3 vertices[], vec3 verticesNormals[], int numIndices, int indices[], int numObjects, int objectsVertices[], int objectsIndices[], vec3 objectsCenterPos[], float objectsRadius[], vec3 objectsColor[], int numLightSources, Light lightSources[])
{
	cl::Event evnt;


	//Write to buffers
	Queue.enqueueWriteBuffer(NumVerticesBuffer, CL_TRUE, 0, sizeof(int), &numvertices);
	Queue.enqueueWriteBuffer(VertexBuffer, CL_TRUE, 0, sizeof(vec3) * numvertices, vertices);
	Queue.enqueueWriteBuffer(VertexNormalBuffer, CL_TRUE, 0, sizeof(vec3) * numvertices, verticesNormals);

	Queue.enqueueWriteBuffer(NumIndicesBuffer, CL_TRUE, 0, sizeof(int), &numIndices);
	Queue.enqueueWriteBuffer(IndexBuffer, CL_TRUE, 0, sizeof(int) * numIndices, indices);

	Queue.enqueueWriteBuffer(numObjBuffer, CL_TRUE, 0, sizeof(int), &numObjects);
	Queue.enqueueWriteBuffer(ObjectsVertexBuffer, CL_TRUE, 0, sizeof(int) * (numObjects + 1), objectsVertices);
	Queue.enqueueWriteBuffer(ObjectsIndexBuffer, CL_TRUE, 0, sizeof(int) * (numObjects + 1), objectsIndices);

	Queue.enqueueWriteBuffer(ObjectsCenterPosBuffer, CL_TRUE, 0, sizeof(vec3) * (numObjects + 1), objectsCenterPos);
	Queue.enqueueWriteBuffer(ObjectsRadiusBuffer, CL_TRUE, 0, sizeof(float) * (numObjects + 1), objectsRadius);
	Queue.enqueueWriteBuffer(ObjectsColorBuffer, CL_TRUE, 0, sizeof(vec3) * (numObjects + 1), objectsColor);

	Queue.enqueueWriteBuffer(NumLightSourcesBuffer, CL_TRUE, 0, sizeof(int), &numLightSources);
	Queue.enqueueWriteBuffer(LightSourcesBuffer, CL_TRUE, 0, sizeof(Light) * (numLightSources + 1), lightSources);



	glFinish();

	//Take ownership of OpenGL texture
	Queue.enqueueAcquireGLObjects(&Screen, NULL, NULL);
	Queue.finish();



	/*//Set kernel arguments
	kernel_RayTracer.setArg(0, NumVerticesBuffer);
	kernel_RayTracer.setArg(1, VertexBuffer);
	kernel_RayTracer.setArg(2, VertexNormalBuffer);

	kernel_RayTracer.setArg(3, NumIndicesBuffer);
	kernel_RayTracer.setArg(4, IndexBuffer);

	kernel_RayTracer.setArg(5, numObjBuffer);
	kernel_RayTracer.setArg(6, ObjectsVertexBuffer);
	kernel_RayTracer.setArg(7, ObjectsIndexBuffer);

	kernel_RayTracer.setArg(8, ObjectsCenterPosBuffer);
	kernel_RayTracer.setArg(9, ObjectsRadiusBuffer);
	kernel_RayTracer.setArg(10, ObjectsColorBuffer);

	kernel_RayTracer.setArg(11, NumLightSourcesBuffer);
	kernel_RayTracer.setArg(12, LightSourcesBuffer); */

	kernel_RayTracer.setArg(0, sizeof(Screen[0]), &Screen[0]);

	int err = CL_SUCCESS;
	//Run kernel
	if (err = Queue.enqueueNDRangeKernel(kernel_RayTracer, cl::NullRange, cl::NDRange(Width, Height), cl::NullRange))
	{
		printf("Could not run kernel!: \n", &err);
		std::cerr << getErrorString(err) << std::endl;
		char c;
		std::cin >> c;
	}

	//Wait for kernel to finnish
	Queue.finish();

	//Give back ownership of OpenGL texture
	Queue.enqueueReleaseGLObjects(&Screen, NULL, NULL);

	Queue.finish();
}


std::string OldTracer::LoadKernelFromFile(std::string fileName)
{
	std::ifstream file(fileName);
	std::string kernel_code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	//convertToString(fileName, kernel_code);
	return kernel_code;
}

bool OldTracer::convertToString(std::string filename, std::string& s)
{
	size_t size;
	char*  str;
	std::fstream f(filename, (std::fstream::in | std::fstream::binary));

	/*if (f.is_open())
	{
	size_t fileSize;
	f.seekg(0, std::fstream::end);
	size = fileSize = (size_t)f.tellg();
	f.seekg(0, std::fstream::beg);
	str = new char[size + 1];
	if (!str)
	{
	f.close();
	return 0;
	}

	f.read(str, fileSize);
	f.close();
	str[size] = '\0';
	s = str;
	delete[] str;
	return true;
	}*/
	s = "\#pragma OPENCL EXTENSION cl_amd_printf : enable"
		"\n"
		"const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_FILTER_NEAREST;"
		"	"
		"	void kernel RayTracer("
		"	global const int* numVertices,"
		"	global const float3* Vertices, "
		"	global const float3* VerticesNormals, "
		"	global const int* numIndices, "
		"	global const int* Indices, "
		"	global const int* numObjects, "
		"	global const int* ObjectsIndice, "
		"	global const int* ObjectsVertex, "
		"	global const float3* ObjectsCenterPos, "
		"	global const float* ObjectsRadius, "
		"	global const float3* ObjectsColor, "
		"	__write_only image2d_t output"
		"	)"
		"{"
		"	uint2 pos (uint2)(get_global_id(0),get_global_id(0)); "
		"	"
		"	//float4 color = (float4)(x / 1000, y / 1000, 0.0f, 1.0f);"
		"	float4 color = (float4)(1.0f, 1.0f, 0.0f, 1.0f); "
		"	\n"
		"	//write_imagef(output, pos, color.xyzw);"
		"	printf(\"%f %f %f %f\n\", color.x, color.y, color.z, color.w); "
		"} ";

	return true;
	std::cout << "Error: failed to open file\n:" << filename << std::endl;
	return false;
}

const std::string OldTracer::getErrorString(cl_int error)
{
	switch (error) {
		// run-time and JIT compiler errors
	case 0: return "CL_SUCCESS";
	case -1: return "CL_DEVICE_NOT_FOUND";
	case -2: return "CL_DEVICE_NOT_AVAILABLE";
	case -3: return "CL_COMPILER_NOT_AVAILABLE";
	case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case -5: return "CL_OUT_OF_RESOURCES";
	case -6: return "CL_OUT_OF_HOST_MEMORY";
	case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case -8: return "CL_MEM_COPY_OVERLAP";
	case -9: return "CL_IMAGE_FORMAT_MISMATCH";
	case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case -11: return "CL_BUILD_PROGRAM_FAILURE";
	case -12: return "CL_MAP_FAILURE";
	case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
	case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case -15: return "CL_COMPILE_PROGRAM_FAILURE";
	case -16: return "CL_LINKER_NOT_AVAILABLE";
	case -17: return "CL_LINK_PROGRAM_FAILURE";
	case -18: return "CL_DEVICE_PARTITION_FAILED";
	case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

		// compile-time errors
	case -30: return "CL_INVALID_VALUE";
	case -31: return "CL_INVALID_DEVICE_TYPE";
	case -32: return "CL_INVALID_PLATFORM";
	case -33: return "CL_INVALID_DEVICE";
	case -34: return "CL_INVALID_CONTEXT";
	case -35: return "CL_INVALID_QUEUE_PROPERTIES";
	case -36: return "CL_INVALID_COMMAND_QUEUE";
	case -37: return "CL_INVALID_HOST_PTR";
	case -38: return "CL_INVALID_MEM_OBJECT";
	case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case -40: return "CL_INVALID_IMAGE_SIZE";
	case -41: return "CL_INVALID_SAMPLER";
	case -42: return "CL_INVALID_BINARY";
	case -43: return "CL_INVALID_BUILD_OPTIONS";
	case -44: return "CL_INVALID_PROGRAM";
	case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
	case -46: return "CL_INVALID_KERNEL_NAME";
	case -47: return "CL_INVALID_KERNEL_DEFINITION";
	case -48: return "CL_INVALID_KERNEL";
	case -49: return "CL_INVALID_ARG_INDEX";
	case -50: return "CL_INVALID_ARG_VALUE";
	case -51: return "CL_INVALID_ARG_SIZE";
	case -52: return "CL_INVALID_KERNEL_ARGS";
	case -53: return "CL_INVALID_WORK_DIMENSION";
	case -54: return "CL_INVALID_WORK_GROUP_SIZE";
	case -55: return "CL_INVALID_WORK_ITEM_SIZE";
	case -56: return "CL_INVALID_GLOBAL_OFFSET";
	case -57: return "CL_INVALID_EVENT_WAIT_LIST";
	case -58: return "CL_INVALID_EVENT";
	case -59: return "CL_INVALID_OPERATION";
	case -60: return "CL_INVALID_GL_OBJECT";
	case -61: return "CL_INVALID_BUFFER_SIZE";
	case -62: return "CL_INVALID_MIP_LEVEL";
	case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
	case -64: return "CL_INVALID_PROPERTY";
	case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
	case -66: return "CL_INVALID_COMPILER_OPTIONS";
	case -67: return "CL_INVALID_LINKER_OPTIONS";
	case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

		// extension errors
	case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
	case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
	case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
	case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
	case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
	case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
	default: return "Unknown OpenCL error";
	}
}