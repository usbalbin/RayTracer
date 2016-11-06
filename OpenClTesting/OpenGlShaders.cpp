#include "stdafx.h"
#include "OpenGlShaders.hpp"

#include "Utils.hpp"
#include "Containers.hpp"

#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

OpenGlShaders::OpenGlShaders(int width, int height, std::string vertexShaderPath, std::string fragmentShaderPath, GLFWframebuffersizefun customResizeCallback) {
	this->width = width;
	this->height = height;
	setupWindow(customResizeCallback);
	this->programID = setupShaders(vertexShaderPath, fragmentShaderPath);
}

OpenGlShaders::~OpenGlShaders() {
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteBuffers(1, &indexBufferID);
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteProgram(programID);

	glfwDestroyWindow(window);
	glfwTerminate();
}

GLuint OpenGlShaders::setupScreen() {
	
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	std::vector<float2> vertices{
		float2(-1.0f, +1.0f),
		float2(+1.0f, +1.0f),

		float2(-1.0f, -1.0f),
		float2(+1.0f, -1.0f)
	};

	//GLuint vertexbuffer;
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float2), vertices.data(), GL_STATIC_DRAW);


	std::vector<unsigned char> indices{
		0, 1, 2,
		2, 1, 3
	};

	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned char), indices.data(), GL_STATIC_DRAW);
	indexCount = indices.size();



	glGenTextures(1, &textureBufferID);
	glBindTexture(GL_TEXTURE_2D, textureBufferID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);


	resizeWindow(width, height);

	return textureBufferID;
}

void OpenGlShaders::resizeWindow(int width, int height) {
	this->width = width;
	this->height = height;

	glBindTexture(GL_TEXTURE_2D, textureBufferID);
	

	//glBindTexture(GL_TEXTURE_2D, textureBufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFinish();
}

void OpenGlShaders::writeToScreen(std::vector<float4> colors) {
	glTexSubImage2D(textureBufferID, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, colors.data());
	glFinish();
}

void OpenGlShaders::setupWindow(GLFWframebuffersizefun customResizeCallback) {
	if (glfwInit() != GL_TRUE)
		exit(1);

	std::string openglVersion = "4.0";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openglVersion[0] - '0');
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openglVersion[2] - '0');

	std::string title = "Title";
	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	if (window == nullptr)
		exit(2);

	int x = 50, y = 50;
	glfwSetWindowPos(window, x, y);
	glfwMakeContextCurrent(window);
	
	glewExperimental = true;
	if(glewInit() != GLEW_OK)
		exit(1);

	glfwSetFramebufferSizeCallback(window, customResizeCallback);
	glClearColor(1.0f, 0.0f, 0.75f, 1.0f);
}

void OpenGlShaders::resizeCallback(GLFWwindow* window, int width, int height) {
	resizeWindow(width, height);
	glViewport(0, 0, width, width);
}

GLuint OpenGlShaders::setupShaders(std::string vertexShaderPath, std::string fragmentShaderPath) {

	GLuint vertexShaderID = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
	GLuint fragmentShaderID = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
	
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	GLint result, logLength;
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		std::vector<char> errorMsg(logLength + 1);
		glGetProgramInfoLog(programID, logLength, nullptr, errorMsg.data());
		std::cout << errorMsg.data() << std::endl;
	}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return programID;
}


GLuint OpenGlShaders::compileShader(std::string shaderPath, GLuint shaderType) {
	std::string shaderCode = readFileToString(shaderPath);
	GLuint shaderID = glCreateShader(shaderType);
	const char* codePtr = shaderCode.c_str();
	glShaderSource(shaderID, 1, &codePtr, nullptr);
	glCompileShader(shaderID);
	GLint result;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	GLint logLength;
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);

	if (logLength > 1) {
		std::vector<char> errorMsg(logLength + 1);
		glGetShaderInfoLog(shaderID, logLength, nullptr, errorMsg.data());
		std::cout << "Failed to compile: " << shaderPath << std::endl;
		std::cout << errorMsg.data() << std::endl;
		exit(4);
	}

	return shaderID;
}

void OpenGlShaders::draw() {

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	// Use our shader
	glUseProgram(programID);

	glBindTexture(GL_TEXTURE_2D, textureBufferID);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangle !
	//glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_BYTE, 0);

	glDisableVertexAttribArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();


}