#pragma once
#include "NOU/Entity.h"
#include "NOU/GLTFLoader.h"
#include "MaterialCreator.h"
using namespace nou;
class OvenTimer
{
public:
	OvenTimer(MaterialCreator& t, MaterialCreator& a, MaterialCreator& c, Transform& pos, float multiplier = 0.3f
	, glm::quat arrowRotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)));
	//Timer(MaterialCreator right, MaterialCreator left, Transform pos);
	void addFill(float);
	float getFill();
	void setFill(float);
	void setMaterial(MaterialCreator&);
	Entity* getTile();
	Entity* getArrow();
	Entity* getCircle();
	void updateArrow(glm::vec3 axis = glm::vec3(0.0f, 0.0f, 1.0f));
	Transform& getTransform();
	void setTransform(Transform&);
	void setPosition(glm::vec3);
	void dontShow(int);//0 for tile, 1 for arrow, 2 for circle
	glm::vec3 getTileScale();
	glm::vec3 getArrowScale();
	glm::vec3 getCircleScale();
protected:
	float fill = 0;
	glm::vec3 tileScale;
	glm::vec3 arrowScale;
	glm::vec3 circleScale;
	std::unique_ptr<Entity> tile;
	std::unique_ptr<Entity> arrow;
	std::unique_ptr<Entity> circle;
	glm::quat arrowRot;
	Transform position;
};

