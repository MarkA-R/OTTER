#pragma once
#include "NOU/Entity.h"
#include "MaterialCreator.h"
#include "NOU/CMeshRenderer.h"
#include <iostream>
using namespace nou;
class Transparency
{
protected:
	float currentTransparency = 0.f;//opaque
	float wantedTransparency = -1.f;
	float beginingTransparency = -1.f;
	float nextWantedTransparency = -1.f;
	float nextTime = 1.f;
	float timeToLERP = 1.f;
	float transparencyT = 0.f;
	glm::vec3 nextPosition = glm::vec3(-999);
	Transform* newParent;
	Transform* newTransPos = nullptr;
	Entity* owner;
	Transform copyPos;
	Transform* posToCopy = nullptr;
	glm::vec3 copiedScale = glm::vec3(-1);
	glm::vec3 nextScale = glm::vec3(-1);
	bool output = false;
public:
	Transparency(Entity&);
	void setTransparency(float);
	void updateTransparency(float);
	float getTransparency();
	void setWantedTransparency(float);
	float getWantedTransparency();
	float getTime();
	void setTime(float);
	void setNextPosition(glm::vec3, Transform*, glm::vec3 scale = glm::vec3(-1));
	void setNextTransformPos(Transform*, Transform*, glm::vec3 scale = glm::vec3(-1));
	void setNextWantedTransparency(float);
	void setNextWantedTime(float);
	void setNextWantedScale(glm::vec3);
	void setOutput(bool);
	bool isFadingIn();
};

