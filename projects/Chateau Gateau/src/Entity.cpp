#include "Entity.h"
Entity::Entity(entt::entity e, VAO shaderProgram) {
	id = entt::registry().alive() + 1;
	ent = e;
	entityVAO = shaderProgram;
	
	//entityVAO.loadData(fileName);
}

Entity::Entity(entt::entity e) {
	id = entt::registry().alive() + 1;
	ent = e;
}

Entity::~Entity() {
	//entt::registry().destroy(id - 1, id - 1);
	//entt::registry().
}

void Entity::attatchShader(Shader s) {
	entityShader = s;
}
int Entity::getID() {
	return id;
}

Shader Entity::getShader() {
	return entityShader;
}
entt::entity Entity::getEntity() {
	return ent;
}

glm::mat4 Entity::getProjection()
{
	calculateProjection();
	return projection;
}

void Entity::setPosition(glm::vec3 x)
{
	position = x;
	calculateProjection();
}

void Entity::setRotation(float x, glm::vec3 a)
{
	rotationAxis = a;
	rotationAngle = x;
	calculateProjection();
}

void Entity::setScale(glm::vec3 x)
{
	scale = x;
	calculateProjection();
}

glm::vec3 Entity::getPosition()
{
	return position;
}


glm::vec3 Entity::getRotationAxis()
{
	return rotationAxis;
}

float Entity::getRotationAngle()
{
	return rotationAngle;
}

glm::vec3 Entity::getScale()
{
	return glm::vec3();
}

void Entity::calculateProjection() {
	projection = glm::scale(projection, scale) * glm::rotate(projection, rotationAngle, rotationAxis) * glm::translate(projection, position);

}

VAO Entity::getVAO()
{
	return entityVAO;
}

