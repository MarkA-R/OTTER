#pragma once
#include <string>
#include "GLincludes.h"
class Shader
{
	
public:
	Shader(std::string, GLenum);
	Shader(std::string, std::string);
	Shader();
	GLuint getShaderProgram();
	std::string getName();
protected:
	GLuint shaderProgram;
	std::string shaderName;
};

