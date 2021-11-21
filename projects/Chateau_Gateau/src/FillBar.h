#pragma once
#include "NOU/Entity.h"
#include "CMorphMeshRenderer.h"
#include "CMorphAnimator.h"
#include "MaterialCreator.h"
using namespace nou;
class FillBar
{
protected:
	std::unique_ptr<Entity> bar;
	Mesh* topBar;
	Mesh* bottomBar;
	MaterialCreator bottomMat;
	MaterialCreator topMat;
	float t = 0.0f;
	Transform position;
	void checkT();
	float maxT = 1.1f;
public:
	void addT(float);
	void setT(float);
	float getT();
	void setup(MaterialCreator& bottom, MaterialCreator& top, Transform pos, float scale = 0.3);
	void updateArrow();
	void setPosition(Transform);
	Entity* getEntity();
	

};

