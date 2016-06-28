#pragma once

#include <string>
#include <GL/glew.h>

class OldShader
{
public:
	OldShader();
	~OldShader();

	void compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

	void linkShaders();

	void addAttribute(const std::string& attributeName);

	void use();

	void unuse();

	GLuint programID;

private:
	int _numAttributes;

	void compileShader(const std::string& filePath, GLuint id);

	GLuint _vertexShaderID;
	GLuint _fragmentShaderID;

};

