
#include "stdafx.h"
#include "Utils.hpp"
#include "glm\detail\func_geometric.hpp"
#include <algorithm>


#include <iostream>
#include <fstream>
#include <queue>

/*inline float3 max(float3 l, float3 r) {
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
}*/



std::string readFileToString(std::string filePath) {
	
	std::string fileContents;
	std::ifstream fileStream = std::ifstream(filePath, std::ifstream::in);
	if (!fileStream.is_open()) {
		std::cout << "Failed to open file: \n" << filePath << std::endl;
		system("pause");
		exit(3);
	}

	std::string line = "";
	while (getline(fileStream, line)) {
		fileContents += "\n" + line;
	}
	fileStream.close();


	return fileContents;
	
}

void writeStringToFile(std::string text, std::string path) {
	std::ofstream file;
	file.open(path);
	file << text;
	file.close();
}

void writeSourcesToFile(cl::Program::Sources sources, std::string path) {
	std::ofstream file;
	file.open(path);
	for (auto source : sources) {
		file << source.first;
	}
	
	file.close();
}

void setColor(std::vector<Vertex>& vertices, float4 color) {
	for (auto& vertex : vertices) {
		vertex.color = color;
	}
}

void calculateNormals(std::vector<Vertex>& vertices, const std::vector<TriangleIndices>& indices) {
	for (auto& triangle : indices) {
		Vertex& a = vertices[triangle.a];
		Vertex& b = vertices[triangle.b];
		Vertex& c = vertices[triangle.c];

		float3 normal = glm::cross(
			a.position - c.position,
			a.position - b.position
		);

		a.normal += normal;
		b.normal += normal;
		c.normal += normal;
	}

	for (auto& vertex : vertices) {
		vertex.normal = glm::normalize(vertex.normal);
	}
}

void readObjFile(std::vector<Vertex>& vertices, std::vector<TriangleIndices>& indices, std::string& filePath, float reflection, float refraction) {
	std::ifstream objFile;
	objFile.open(filePath);

	std::string line;

	std::vector<float3> positions;
	//std::vector<float2> texturePositions;
	std::vector<float3> normals;
	std::vector<Face> faces;

	std::map<Vertex, int> vertexMap;

	while (getline(objFile, line)) {
		if (line.find("v ") != line.npos)
			positions.push_back(parseFloat3(line.substr(2)));
		
		else if (line.find("vn ") != line.npos)
			normals.push_back(parseFloat3(line.substr(3)));


		else if (line.find("f ") != line.npos) {
			faces.push_back(parseFace(line.substr(2)));
		}
	}

	for (Face& face : faces) {

		TriangleIndices triangleIndices;

		addVertex(std::get<0>(face), &triangleIndices.a, vertexMap, vertices, positions, normals, reflection, refraction);

		addVertex(std::get<1>(face), &triangleIndices.b, vertexMap, vertices, positions, normals, reflection, refraction);

		addVertex(std::get<2>(face), &triangleIndices.c, vertexMap, vertices, positions, normals, reflection, refraction);

		indices.push_back(triangleIndices);
	}
	int max = -1, min = INT_MAX;
	for (TriangleIndices triIndices : indices) {
		if (triIndices.a >= vertices.size() || triIndices.a < 0)
			throw;
		if (triIndices.b >= vertices.size() || triIndices.b < 0)
			throw;
		if (triIndices.c >= vertices.size() || triIndices.c < 0)
			throw;
		max = std::max({ max, triIndices.a, triIndices.b, triIndices.c });
		min = std::min({ min, triIndices.a, triIndices.b, triIndices.c });
	}

	float3 maxPos(-FLT_MAX, -FLT_MAX, -FLT_MAX), minPos(FLT_MAX, FLT_MAX, FLT_MAX);
	for (auto v : vertices) {
		maxPos = float3(
			std::max(maxPos.x, v.position.x),
			std::max(maxPos.y, v.position.y),
			std::max(maxPos.z, v.position.z)
		);
		minPos = float3(
			std::min(minPos.x, v.position.x),
			std::min(minPos.y, v.position.y),
			std::min(minPos.z, v.position.z)
		);
	}
	int x = 0;
}

void addVertex(FaceElement facePart, int* indexOut, std::map<Vertex, int>& vertexMap, std::vector<Vertex>& vertices, std::vector<float3>& positions,/* std::vector<float3>& texturePositions,*/ std::vector<float3>& normals, float reflection, float refraction) {
	Vertex vertex = facePartToVertex(facePart, positions, normals);
	vertex.reflectFactor = reflection;
	vertex.refractFactor = refraction;

	if (vertexMap.find(vertex) == vertexMap.end()) {
		*indexOut = vertices.size();
		vertices.push_back(vertex);
		vertexMap[vertex] = *indexOut;
	}
	else
		*indexOut = vertexMap[vertex];
}

Vertex facePartToVertex(FaceElement facePart, std::vector<float3>& positions,/* std::vector<float3>& texturePositions,*/ std::vector<float3>& normals) {
	Vertex vertex;
	vertex.position = positions[std::get<0>(facePart)];
	vertex.color = float4(0.2f);

	//vertex.uv = texturePositions[std::get<1>(facePart)]
	if (std::get<2>(facePart) != -1)
		vertex.normal = normals[std::get<2>(facePart)];
	else
		vertex.normal = float3(0.0f, 0.0f, 0.0f);
	return vertex;
}

Face parseFace(std::string line) {
	Face result;

	size_t start = 0, end;
		
	end = line.find(" ");
	std::string part = line.substr(start, end);
	std::get<0>(result) = parseFacePart(part);
	start = std::min(end + 1, line.npos);
	
	end = line.find(" ", start);
	part = line.substr(start, end);
	std::get<1>(result) = parseFacePart(part);
	start = std::min(end + 1, line.npos);

	
	part = line.substr(start);
	std::get<2>(result) = parseFacePart(part);


	return result;
}

FaceElement parseFacePart(std::string line) {
	FaceElement result;

	int slashCount = std::count(line.begin(), line.end(), '/') + 1;
	int start = 0, end;

	//Position
	end = line.find("/");
	std::string part = line.substr(start, end - start);
	std::get<0>(result) = std::stoi(part) - 1;
	start = end + 1;

	//Texture position
	if (slashCount > 1 && line.find("//") == line.npos) {
		end = line.find("/", start);
		part = line.substr(start, end - start);
		std::get<1>(result) = std::stoi(part) - 1;
		start = end + 1;
	} else std::get<1>(result) = -1;

	//Normal
	if (slashCount > 2) {
		if (line.find("//") != line.npos)
			start++;
		part = line.substr(start);
		std::get<2>(result) = std::stoi(part) - 1;
	} else std::get<2>(result) = -1;
	
	return result;
}

float3 parseFloat3(std::string str) {
	float3 result;
	size_t pos;

	for (int i = 0; i < 3; i++) {
		result[i] = std::stof(str, &pos);
		str = str.substr(pos);
	}
	return result;
}

void splitMesh(std::vector<Vertex>& vertices, std::vector<TriangleIndices>& indices, std::vector<Vertex>& verticesOut, std::vector<TriangleIndices>& indicesOut)
{
	std::vector<Vertex> tempVertices;
	std::vector<TriangleIndices> tempIndices;

	AABB aabb(vertices);
	float3 delta = aabb.max - aabb.min;

	int splitDimension = 0;
	float widestDimension = delta.x;
	if (delta.y > widestDimension) {
		widestDimension = delta.y;
		splitDimension = 1;
	}
	if (delta.z > widestDimension) {
		widestDimension = delta.z;
		splitDimension = 2;
	}

	auto sortingFunction = [&](TriangleIndices& thisTriangleIndex, TriangleIndices& otherTriangleIndex) {
		float3 thisCenter = AABB(std::vector<float3>{
			vertices[thisTriangleIndex.a].position,
			vertices[thisTriangleIndex.b].position,
			vertices[thisTriangleIndex.c].position
		}).center();

		float3 otherCenter = AABB(std::vector<float3>{
			vertices[otherTriangleIndex.a].position,
			vertices[otherTriangleIndex.b].position,
			vertices[otherTriangleIndex.c].position
		}).center();

		return thisCenter[splitDimension] < otherCenter[splitDimension];
	};

	std::sort(indices.begin(), indices.end(), sortingFunction);

	std::map<int, int> dictionary;
	for (int i = 0; i < indices.size() / 2; i++) {
		TriangleIndices triIndex = indices[i];
		TriangleIndices translatedTriIndex;
		for (int j = 0; j < 3; j++) {
			if (dictionary.find(triIndex[j]) != dictionary.end()) {
				translatedTriIndex[j] = dictionary[triIndex[j]];
			}
			else {
				dictionary[triIndex[j]] = translatedTriIndex[j] = tempVertices.size();
				tempVertices.push_back(vertices[triIndex[j]]);
			}
		}
		tempIndices.push_back(translatedTriIndex);
	}
	dictionary.clear();
	for (int i = indices.size() / 2; i < indices.size(); i++) {
		TriangleIndices triIndex = indices[i];
		TriangleIndices translatedTriIndex;
		for (int j = 0; j < 3; j++) {
			if (dictionary.find(triIndex[j]) != dictionary.end()) {
				translatedTriIndex[j] = dictionary[triIndex[j]];
			}
			else {
				dictionary[triIndex[j]] = translatedTriIndex[j] = verticesOut.size();
				verticesOut.push_back(vertices[triIndex[j]]);
			}
		}
		indicesOut.push_back(translatedTriIndex);
	}

	vertices = tempVertices;
	indices = tempIndices;
}