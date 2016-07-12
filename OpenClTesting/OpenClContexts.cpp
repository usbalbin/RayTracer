#include "stdafx.h"
#include "OpenClContexts.hpp"

#ifdef _WIN32
	#include <Windows.h>
#elif __linux__
	//To do add linux stuff
#endif

#include <iostream>

void OpenClContexts::initializeAll(){
	initializeCpu();
	initializeGpu();
}

void OpenClContexts::initializeGpu(){
	initialize(CL_DEVICE_TYPE_GPU);
}

void OpenClContexts::initializeCpu(){
	initialize(CL_DEVICE_TYPE_CPU);
}


void OpenClContexts::initializeInteropGpu() {
	if (gpuContexts.size() > 0)
		return;

	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	int numPlatforms = platforms.size();

	if (numPlatforms == 0) {
		std::cout << "No OpenCL compatible platforms found" << std::endl;
		exit(1);
	}

	std::cout << std::endl;
	std::cout << "---------------- GPU ----------------" << std::endl;
	std::cout << "-------------------------------" << std::endl;
	std::vector<cl::Device> platformDevices;
	for (int i = 0; i < numPlatforms; i++) {
		try {
			platforms[i].getDevices(CL_DEVICE_TYPE_GPU, &platformDevices);
		}
		catch (cl::Error) {
			continue;
		}

		for(auto device : platformDevices) {
			gpuDevices.push_back(device);

			std::cout << device.getInfo<CL_DEVICE_VENDOR>() << std::endl;
			std::cout << device.getInfo<CL_DEVICE_NAME>() << std::endl;
			std::cout << "-------------------------------" << std::endl;





#ifdef _WIN32

			cl_context_properties contextProperties[] = {
				CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[i](),
				CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
				CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
				NULL
			};
#elif __linux__
			cl_context_properties contextProperties[] = {
				CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[i](),
				CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
				CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
				NULL
			};
#endif


			int status;
			cl::Context context(device, contextProperties, NULL, NULL, &status);
			
			if (status == CL_SUCCESS)
				gpuContexts.push_back(context);
		}
		platformDevices.clear();
	}
	std::cout << std::endl;

	if (gpuDevices.size() == 0) {
		std::cout << "No supported computing units available or No OpenGL context created" << std::endl;
		exit(1);
	}

	
}


//TODO initializeCpu()

void OpenClContexts::initialize(int deviceType){
	if(gpuContexts.size() > 0)
		return;
	
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	
	int numPlatforms = platforms.size();
	
	if(numPlatforms == 0){
		std::cout << "No OpenCL compatible platforms found" << std::endl;
		exit(1);
	}
	
	std::vector<cl::Device>* devices;
	std::vector<cl::Context>* contexts;
	
	if(deviceType == CL_DEVICE_TYPE_GPU){
		devices = &gpuDevices;
		contexts = &gpuContexts;
	}
	else if(deviceType == CL_DEVICE_TYPE_CPU){
		devices = &cpuDevices;
		contexts = &cpuContexts;
	}
	else{
		std::cout << "Unsupported device type" << std::endl;
		exit(1);
	}
	
	std::cout << std::endl;
	std::cout << "---------------- " << (deviceType == CL_DEVICE_TYPE_GPU ? "GPU" : "CPU") << "-Devices" << " ----------------" << std::endl;
	std::cout << "-------------------------------" << std::endl;
	std::vector<cl::Device> platformDevices;
	for(int i = 0; i < numPlatforms; i++){
		platforms[i].getDevices(deviceType, &platformDevices);
		for(auto device : platformDevices){
			devices->push_back(device);
			
			std::cout << device.getInfo<CL_DEVICE_VENDOR>() << std::endl;
			std::cout << device.getInfo<CL_DEVICE_NAME>() << std::endl;
			std::cout << "-------------------------------" << std::endl;
		}
		platformDevices.clear();
	}
	std::cout << std::endl;
	
	if(devices->size() == 0){
		std::cout << "No supported computing units available" << std::endl;
		exit(1);
	}
	
	if(deviceType == CL_DEVICE_TYPE_GPU || deviceType == CL_DEVICE_TYPE_CPU){
		for(auto device : *devices){
			cl::Context context(device);
			contexts->push_back(context);
		}
	}
	else{
		std::cout << "Unsupported device type" << std::endl;
		exit(1);
	}
}

cl::Device& OpenClContexts::getGpuDevice(int deviceNum){
	return gpuDevices[deviceNum];
}

cl::Context& OpenClContexts::getGpuContext(int contextNum){
	return gpuContexts[contextNum];
}

cl::Device& OpenClContexts::getCpuDevice(int deviceNum){
	return cpuDevices[deviceNum];
}

cl::Context& OpenClContexts::getCpuContext(int contextNum){
	return cpuContexts[contextNum];
}