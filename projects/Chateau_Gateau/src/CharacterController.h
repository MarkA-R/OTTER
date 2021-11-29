#pragma once
#include "MorphAnimation.h"
#include "CMorphMeshRenderer.h"
#include "CMorphAnimator.h"
#include "NOU/Entity.h"
#include "NOU/CMeshRenderer.h"
#include "MotionTable.h"
using namespace nou;
class CharacterController
{
protected:
	
	std::vector<MorphAnimation*> animations;
	//0 for idle, 1 for walk, 2 for something else and so on
	Mesh* startTransition;
	Mesh* endTransition;
	float transitionT = 0.f;
	float transitionTAdder = 0.f;
	bool inTransition = false;
	int currentspot = 0;
	int nextSpot = 1;
	int stopSpot = 1;
	float lineT = 0.f;
	int currentAnimation = 0;
	int queuedAnimation = -1;
	bool stopAnimation = false;
	std::vector<glm::vec3> linePositions;
	Entity* owner;
	float distanceTravelled = 0.f;
	MotionTable table;
	int currentTable = 0;
	int nextTable = 1;
public:
	CharacterController(Entity*, std::vector<MorphAnimation*>, std::vector<glm::vec3>);
	void setCurrentSpot(int);
	void setStopSpot(int);
	void queueAnimation(int);
	void updateAnimation(float);
	void updatePosition(float, float);
	void updateDistance(float deltaTime, float speed);
	void continueAnimation(bool);
	int getStopSpot();

};

