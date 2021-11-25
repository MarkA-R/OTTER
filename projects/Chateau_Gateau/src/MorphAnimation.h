#pragma once
#include "NOU/Entity.h"
#include "NOU/CMeshRenderer.h"
using namespace nou;
class MorphAnimation
{
protected:
	std::vector<Mesh*> frames;
	int currentFrame = 0;
	int nextFrame = 0;
	float frameTime = 1.f;//time between frames in seconds
	float currentT = 0.f;
	int timesLooped = 0;
public:
	MorphAnimation(std::vector<Mesh*>, float, int startingFrame = 0);
	void update(Entity*, float, bool stopAtEnd = false);
	int getCurrentFrame();
	int getNextFrame();
	float getT();
	float getAddedT(float);
	float getFrameTime();
	Mesh* getMeshAtFrame(int i);


};

