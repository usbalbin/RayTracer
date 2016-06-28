#pragma once

#include "stdafx.h"

#include <iostream>
#include <vector>
#include <string>

#include "CL/cl.hpp"


template<typename T, typename... Ts> 
class ParallelCompute {
	public:
		ParallelCompute(cl::Context context, cl::Device device, std::string globals, std::string args, std::string code);
		ParallelCompute<T, Ts...>& resizeBuffers(std::vector<int> inputsElementCount, int outputElementCount, int threadCount);
		std::vector<T> compute(std::vector<Ts>&... inputs);
		cl::Event computeNonBlocking(std::vector<Ts>&... inputs);
		std::vector<T> fetchResult();
		
	private:
		cl::Kernel computeKernel;
		cl::Context context;
		cl::CommandQueue queue;
		
		std::vector<cl::Buffer> buffers;
		
		cl::Buffer resBuffer;
		int threadCount;
		int outputElementCount = 0;
		
};


template<typename T, typename... Ts>
ParallelCompute<T, Ts...>::ParallelCompute(cl::Context context, cl::Device device, std::string globals, std::string args, std::string code){
	this->context = context;	
	
	
	cl::Program::Sources sources;
	
	std::string sourceCode = 
		globals +
		"void kernel computeKernel(" + args + "){ \n"
		"    int i = get_global_id(0);\n"
		"    " + code + "\n"
		"}\n";
	
	sources.push_back({sourceCode.c_str(), sourceCode.length()});

	cl::Program program(context, sources);
	
	
	if(program.build({device}) != CL_SUCCESS){
		std::cout << "Failed to compile: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
		exit(1);
	}
	
	queue = cl::CommandQueue(context, device, NULL, NULL);
		
	computeKernel = cl::Kernel(program, "computeKernel");
}

template<typename T, typename... Ts>
ParallelCompute<T, Ts...>& ParallelCompute<T, Ts...>::resizeBuffers(std::vector<int> inputsElementCount, int outputElementCount, int threadCount){
	this->outputElementCount = outputElementCount;
	this->threadCount = threadCount;
	buffers.resize(inputsElementCount.size());

	for(int i = 0; i < buffers.size(); i++){
		buffers[i] = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(T) * inputsElementCount[i]);
	}

	resBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(T) *  outputElementCount);	
	
	return *this;
}

template<typename T, typename... Ts>
std::vector<T> ParallelCompute<T, Ts...>::compute(std::vector<Ts>&... inputs){
	computeNonBlocking(inputs...).wait();
	return fetchResult();
}

template<typename T, typename... Ts>
cl::Event ParallelCompute<T, Ts...>::computeNonBlocking(std::vector<Ts>&... inputs){
	std::vector<std::vector<T>> inputList = { inputs... };
	int i;
	for(i = 0; i < inputList.size(); i++){
		if(queue.enqueueWriteBuffer(buffers[i], CL_TRUE, 0, sizeof(T) * inputList[i].size(), inputList[i].data()) != CL_SUCCESS){
			std::cout << "Failed to write to buffer" << std::endl;
			exit(1);
		}
		computeKernel.setArg(i, buffers[i]);
	}
	
	computeKernel.setArg(i, resBuffer);
	
	queue.finish();
	
	cl::Event event;
	
	queue.enqueueNDRangeKernel(computeKernel, cl::NullRange, cl::NDRange(threadCount), cl::NullRange, 0, &event);
	
	return event;
}

template<typename T, typename... Ts>
std::vector<T> ParallelCompute<T, Ts...>::fetchResult(){
	std::vector<T> res(outputElementCount);
	if(outputElementCount > 0){
		if(queue.enqueueReadBuffer(resBuffer, CL_TRUE, 0, sizeof(T) * outputElementCount, res.data()) != CL_SUCCESS){
			std::cout << "Failed to read from buffer" << std::endl;
			exit(1);
		}
	}
		
	std::cout << std::endl << "Result fetched" << std::endl;
	return res;
}