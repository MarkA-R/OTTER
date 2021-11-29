#pragma once
#include "NOU/Entity.h"
using namespace nou;
class Transparency
{
protected:
	float currentTransparency = 0.f;//opaque
	float wantedTransparency = -1.f;
	float timeToLERP = 1.f;
	float transparencyT = 0.f;
	glm::vec3 nextPosition = glm::vec3(-999);
	Transform* newParent;
	Entity* owner;
public:
	Transparency(Entity&);
	void setTransparency(float);
	void updateTransparency(float);
	float getTransparency();
	void setWantedTransparency(float, float time = 1.f);
	float getWantedTransparency();
	float getTime();
	void setTime(float);
	void setNextPosition(glm::vec3, Transform*);
	
};

