
#include "OldShader.h"
#include "Errors.h"

#include <fstream>
#include <vector>

OldShader::OldShader() : _numAttributes(0), programID(0), _vertexShaderID(0), _fragmentShaderID(0)
{
}


OldShader::~OldShader()
{
}

void OldShader::compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
{
	_vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	if (_vertexShaderID == 0)
		fatalError("Vertex Shader failed to be created");

	_fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	if (_fragmentShaderID == 0)
		fatalError("Vertex Shader failed to be created");

	std::ifstream vertexFile(vertexShaderFilePath);
	if (vertexFile.fail())
	{
		perror(vertexShaderFilePath.c_str());
		fatalError("Failed to open " + vertexShaderFilePath);
	}

	compileShader(vertexShaderFilePath, _vertexShaderID);

	compileShader(fragmentShaderFilePath, _fragmentShaderID);
}

void OldShader::linkShaders()
{
	//Vertex and fragment shaders are successfully compiled.
	//Now time to link them together into a program.
	//Get a program object.
	programID = glCreateProgram();

	//Attach our shaders to our program
	glAttachShader(programID, _vertexShaderID);
	glAttachShader(programID, _fragmentShaderID);

	//Link our program
	glLinkProgram(programID);

	//Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<char> errorLog(maxLength);
		glGetProgramInfoLog(programID, maxLength, &maxLength, &errorLog[0]);

		std::printf("%s\n", &errorLog[0]);
		fatalError("Shader failed to link!");

		//We don't need the program anymore.
		glDeleteProgram(programID);
		//Don't leak shaders either.
		glDeleteShader(_vertexShaderID);
		glDeleteShader(_fragmentShaderID);

		//Use the infoLog as you see fit.

		//In this simple program, we'll just leave
		return;
	}

	//Always detach shaders after a successful link.
	glDetachShader(programID, _vertexShaderID);
	glDetachShader(programID, _fragmentShaderID);
	glDeleteShader(_vertexShaderID);
	glDeleteShader(_fragmentShaderID);
}

void OldShader::addAttribute(const std::string& attributeName)
{
	glBindAttribLocation(programID, _numAttributes++, attributeName.c_str());
}

void OldShader::use()
{
	glUseProgram(programID);
	//for (int i = 0; i < _numAttributes; i++) //http://antongerdelan.net/opengl/vertexbuffers.html
	//{
	//	glEnableVertexAttribArray(i);
	//}
}

void OldShader::unuse()
{
	glUseProgram(0);
	//for (int i = 0; i < _numAttributes; i++) //http://antongerdelan.net/opengl/vertexbuffers.html
	//{
	//	glDisableVertexAttribArray(i);
	//}
}

void OldShader::compileShader(const std::string& filePath, GLuint id)
{
	std::ifstream vertexFile(filePath);
	if (vertexFile.fail())
	{
		perror(filePath.c_str());
		fatalError("Failed to open " + filePath);
	}

	std::string fileContents = "";
	std::string line;

	while (std::getline(vertexFile, line))
		fileContents += line + "\n";

	vertexFile.close();

	const char* contentPtr = fileContents.c_str();
	glShaderSource(id, 1, &contentPtr, nullptr);

	glCompileShader(id);

	GLint success = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);


		glDeleteShader(id);

		std::printf("%s\n", &errorLog[0]);
		fatalError("Shader " + filePath + " failed to compile");
	}
}