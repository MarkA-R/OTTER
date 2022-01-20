#include "MorphAnimation.h"
#include "CMorphMeshRenderer.h"
#include "CMorphAnimator.h"

MorphAnimation::MorphAnimation(std::vector<Mesh*> meshes, float nextFrameTime, int startingFrame)
{
	frames = meshes;
	frameTime = nextFrameTime;
	currentFrame = startingFrame;
	nextFrame = currentFrame + 1;

}

void MorphAnimation::update(Entity* e, float deltaTime, bool stopAtEnd)
{
	currentT += deltaTime / frameTime;
	while (currentT >= 1) {
		currentT -= 1;	
		
		
		

		
		currentFrame++;
		nextFrame++;
		
		timesLooped++;
		
	}
	

	if (currentFrame >= (frames.size())) {
		currentFrame = currentFrame % frames.size();
	}
	

	if (nextFrame >= (frames.size())) {
		nextFrame = (currentFrame + 1) % frames.size();
	}

	if (stopAtEnd && timesLooped >= frames.size()) {

		currentFrame = frames.size() - 1;
		nextFrame = 0;
		currentT = 1.0;
	}
	
	if (e->Has<CMorphMeshRenderer>()) {
		e->Get<CMorphAnimator>().setFrameAndTime(currentFrame, nextFrame, currentT);
	}
}

int MorphAnimation::getCurrentFrame()
{
	return currentFrame;
}

int MorphAnimation::getNextFrame()
{
	return nextFrame;
}

void MorphAnimation::setCurrentFrame(int x)
{
	currentFrame = x;
	nextFrame = currentFrame + 1;
	if (currentFrame >= (frames.size())) {
		currentFrame = currentFrame % frames.size();
	}


	if (nextFrame >= (frames.size())) {
		nextFrame = (currentFrame + 1) % frames.size();
	}
}

float MorphAnimation::getT()
{
	return currentT;
}

void MorphAnimation::setT(float x)
{
	currentT = x;
}

float MorphAnimation::getAddedT(float deltaTime)
{
	return deltaTime / frameTime;
}

float MorphAnimation::getFrameTime()
{
	return frameTime;
}

Mesh* MorphAnimation::getMeshAtFrame(int i)
{
	return frames[i];
}

std::vector<Mesh*> MorphAnimation::getFrames()
{
	return frames;
}

int MorphAnimation::getFrameSize()
{
	return frames.size();
}

void MorphAnimation::reverseFrames()
{
	std::reverse(frames.begin(), frames.end());
}
