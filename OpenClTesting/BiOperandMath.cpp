#include "BiOperandMath.hpp"

#include <iostream>


BiOperandMath::BiOperandMath(cl::Context context, cl::Device device, std::string args, std::string code){
	this->context = context;

	cl::Program::Sources sources;
	
	std::string sourceCode = 
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

std::vector<float> BiOperandMath::compute(std::vector<float>& a, std::vector<float>& b){
	computeNonBlocking(a, b).wait();
	return fetchResult();
}

cl::Event BiOperandMath::computeNonBlocking(std::vector<float>& a, std::vector<float>& b){
	if(a.size() != b.size()){
		std::cout << "a and b and res should all be the same size" << std::endl;
		exit(1);
	}
	int size = a.size();
	
	
	if(size != elementCount || elementCount == 0){
		aBuffer   = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float) * size);
		bBuffer   = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float) * size);
		resBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float) * size);
		elementCount = size;
	}
	
	if(queue.enqueueWriteBuffer(aBuffer, CL_TRUE, 0, sizeof(float) * size, a.data()) != CL_SUCCESS){
		std::cout << "Failed to write to buffer" << std::endl;
		exit(1);
	}
	
	if(queue.enqueueWriteBuffer(bBuffer, CL_TRUE, 0, sizeof(float) * size, b.data()) != CL_SUCCESS){
		std::cout << "Failed to write to buffer" << std::endl;
		exit(1);
	}
	
	computeKernel.setArg(0, aBuffer);
	computeKernel.setArg(1, bBuffer);
	computeKernel.setArg(2, resBuffer);
	
	queue.finish();
	
	cl::Event event;
	
	queue.enqueueNDRangeKernel(computeKernel, cl::NullRange, cl::NDRange(size), cl::NullRange, 0, &event);
	
	return event;
}

std::vector<float> BiOperandMath::fetchResult(){
	float values[10];
	std::vector<float> res(elementCount);
	if(elementCount > 0){
		if(queue.enqueueReadBuffer(resBuffer, CL_TRUE, 0, sizeof(float) * elementCount, res.data()) != CL_SUCCESS){
			std::cout << "Failed to read from buffer" << std::endl;
			exit(1);
		}
	}
		
	std::cout << std::endl << "Result fetched" << std::endl;
	return res;
}
