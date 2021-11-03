#pragma once
#include "GLFW/glfw3.h"
#include "GLM/glm.hpp"
#include "NOU/Entity.h"
using namespace nou;
class BoundingBox
{
public:
	BoundingBox(float, Entity&);
	BoundingBox(glm::vec3, Entity&);
	BoundingBox(float, float, float, Entity&);
	glm::vec3 getOrigin();
	bool isColliding(glm::vec3); //returns if the point is in the bounding box
	float reachX;//how far the X will go FROM THE ORIGIN
	float reachY;
	float reachZ;//this is for the min and max axis using AABB
	
protected:
	Entity* owner;
};

