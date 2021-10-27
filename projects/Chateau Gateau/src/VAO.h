#pragma once
#include "GLincludes.h"
#include "Shader.h"

class VAO
{

public:
	VAO();
	
	
	void loadData(std::string);
	void Draw(glm::mat4 , glm::mat4 , glm::mat4, Shader );
	GLuint getUniformLocation();
protected:
	int trisToDraw;
	GLuint uniformLocation;
	Shader shader;
};

