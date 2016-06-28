#pragma once

#define GLEW_STATIC
#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include <string>

class OpenGlShaders
{
public:
	OpenGlShaders();
	OpenGlShaders(int width, int height, std::string vertexShaderPath, std::string fragmentShaderPath);
	~OpenGlShaders();
	GLuint setupInteropScreen();
	void draw();
	GLFWwindow* getWindow() { return window; }

private:
	void setupWindow();
	GLuint setupShaders(std::string vertexShaderPath, std::string fragmentShaderPath);
	GLuint compileShader(std::string shaderPath, GLuint shaderType);

	GLFWwindow* window = nullptr;
	GLuint programID = 0;
	GLuint vertexBufferID = 0;
	GLuint indexBufferID = 0;
	GLuint textureBufferID;
	GLuint vertexArrayID = 0;
	GLint positionIndex = 0;
	int indexCount = 6;

	int width;
	int height;
};

