#pragma once
#include "NOU/Entity.h"
#include "MaterialCreator.h"
using namespace nou;
class Transparency
{
protected:
	float currentTransparency = 0.f;//opaque
	float wantedTransparency = -1.f;
	float beginingTransparency = -1.f;
	float timeToLERP = 1.f;
	float transparencyT = 0.f;
	glm::vec3 nextPosition = glm::vec3(-999);
	Transform* newParent;
	Entity* owner;
	Entity* inverseCopy = nullptr;
	
public:
	Transparency(Entity&);
	void setTransparency(float);
	void updateTransparency(float);
	float getTransparency();
	void setWantedTransparency(float);
	float getWantedTransparency();
	float getTime();
	void setTime(float);
	void setNextPosition(glm::vec3, Transform*);
	void setInverseCopy(Transform*, MaterialCreator*, bool setParent = false);
	Entity* getInverseCopy();
};

