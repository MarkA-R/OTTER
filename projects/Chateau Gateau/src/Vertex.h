#pragma once
#include "globalHeaders.h"
class Vertex
{
	//VertexPosNormTexCol
public:
	Vertex(glm::vec3,glm::vec3, glm::vec2, glm::vec4);
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec4 colour;

	
};

