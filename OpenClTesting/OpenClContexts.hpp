#pragma once

#include <vector>
#include "CL/cl.hpp"

class OpenClContexts {

	public:
		void initializeAll();
		void initializeGpu();
		void initializeCpu();
		void initializeInteropGpu();
		cl::Device getGpuDevice(int deviceNum);
		cl::Context getGpuContext(int contextNum);
		cl::Device getCpuDevice(int deviceNum);
		cl::Context getCpuContext(int contextNum);
		int getGpuContextCount();
		int elementCount = 0;
	
	private:
		void initialize(int deviceType);
		std::vector<cl::Device> gpuDevices;
		std::vector<cl::Context> gpuContexts;
		std::vector<cl::Device> cpuDevices;
		std::vector<cl::Context> cpuContexts;
};
