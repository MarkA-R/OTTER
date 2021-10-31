#pragma once
#include "glHeaders.h"
class Transform
{
public:
	Transform();
	void setPosition(glm::vec3);
	void addPosition(glm::vec3);
	void addRotation(glm::vec4);
	void setRotation(glm::vec4);
	void setScale(glm::vec3);
	glm::vec3 getPosition();
	glm::vec4 getRotation();
	glm::vec3 getScale();
	glm::vec3 getEuler(glm::vec4);
	glm::vec4 getQuaternion(glm::vec3, float);
	glm::vec3 getRotationAxis();
	float getAxisAngle();
	glm::mat4 getMatrix();
protected:
	glm::vec3 pos;
	glm::vec3 scale;
	glm::vec4 rotation;


};

