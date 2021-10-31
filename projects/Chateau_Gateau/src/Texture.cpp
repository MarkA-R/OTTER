#include "Texture.h"
#include<stb_image.h>



void Texture::setTexture(std::string fileName, GLuint* usingInt)
{
	binder = usingInt;
	int channels;
	//open gl loads image flipped
	stbi_set_flip_vertically_on_load(true);//cause opengl loads it in flipped

	//load image
	image = stbi_load(fileName.c_str(), &width, &height, &channels, 0);
	if (image) {
		std::cout << "image loaded: " << width << " " << height << std::endl;
	}
	else
	{
		std::cout << "image failed to load - " << fileName << std::endl;
	}

	glGenTextures(1, usingInt);
	glBindTexture(GL_TEXTURE_2D, *usingInt);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

unsigned char* Texture::getTexture()
{
	return image;
}

GLuint* Texture::getuint()
{
	return binder;
}
