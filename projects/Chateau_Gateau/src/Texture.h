#pragma once
#include "glHeaders.h";
class Texture
{
public:
	void setTexture(std::string, GLuint*);
	unsigned char* getTexture();
	GLuint* getuint();
protected:
	unsigned char* image;
	GLuint* binder;
	int width;
	int height;
};

