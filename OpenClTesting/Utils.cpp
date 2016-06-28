
#include "stdafx.h"
#include "Utils.hpp"
#include <algorithm>

#include <iostream>
#include <fstream>

inline float3 max(float3 l, float3 r) {
	return float3(
		std::max(l.x, r.x),
		std::max(l.y, r.y),
		std::max(l.z, r.z)
	);
}

inline float3 min(float3 l, float3 r) {
	return float3(
		std::min(l.x, r.x),
		std::min(l.y, r.y),
		std::min(l.z, r.z)
	);
}



std::string readFileToString(std::string filePath) {
	
	std::string fileContents;
	std::ifstream fileStream = std::ifstream(filePath, std::ifstream::in);
	if (!fileStream.is_open()) {
		std::cout << "Failed to open file: \n" << filePath << std::endl;
		exit(3);
	}

	std::string line = "";
	while (getline(fileStream, line)) {
		fileContents += "\n" + line;
	}
	fileStream.close();

	return fileContents;
	
}