#pragma once
#include "glHeaders.h"
#include <string>
#include "Transform.h"
#include "Texture.h"
#include "VBO.h"
class MeshRenderer
{
public:
	void loadOBJ(std::string, VBO&);
	void loadTexture(Texture);
	void draw(Transform*, GLuint);
	int getTrisToDraw();
	GLuint setMatrixModel(GLuint, std::string);

protected: 
	
	unsigned char* image;
	int width;
	int height;
	int trisToDraw;
	GLuint matrixModel;
	GLuint vao;
	
};

