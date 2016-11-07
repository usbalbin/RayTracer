#pragma once

#include "Containers.hpp"
#include <string>
#include <CL/cl.hpp>

//inline float3 max(float3 l, float3 r);
//inline float3 min(float3 l, float3 r);
std::string readFileToString(std::string filePath);

void writeStringToFile(std::string text, std::string path);
void writeSourcesToFile(cl::Program::Sources sources, std::string path);
