#pragma once
#include "GLincludes.h"
#include "VAO.h"

class Entity
{
public:
	Entity(entt::entity, VAO);
	Entity(entt::entity);
	~Entity();
	int getID();
	entt::entity getEntity();
	glm::mat4 getProjection();
	void setPosition(glm::vec3);
	void setRotation(float, glm::vec3);
	void setScale(glm::vec3);
	glm::vec3 getPosition();
	//glm::vec3 getRotation();//change to a quaternion later
	glm::vec3 getRotationAxis();
	float getRotationAngle();//in radians
	glm::vec3 getScale();
	void calculateProjection();
	VAO getVAO();
	Shader getShader();
	void attatchShader(Shader);
	//set shader functions?

protected:
	int id;
	entt::entity ent;
	glm::mat4 projection;
	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotationAxis = glm::vec3(0);
	float rotationAngle = 0;
	glm::vec3 scale = glm::vec3(1);
	VAO entityVAO;
	Shader entityShader;
	void setValues();

};

