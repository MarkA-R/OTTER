#pragma once
#include "GLFW/glfw3.h"
#include "GLM/glm.hpp"
#include "NOU/Entity.h"
using namespace nou;
class Raycast
{
public:
	
	Raycast(glm::vec3, glm::vec3, float d = 5, float s = 0.05);
	std::vector<glm::vec3> crossedPoints();
	float step;
	float distance;
	glm::vec3 direction;
	glm::vec3 origin;

};

