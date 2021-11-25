#include "CharacterController.h"

CharacterController::CharacterController(Entity* e, std::vector<MorphAnimation*> toPlay, std::vector<glm::vec3> toMove)
{
	owner = e;
	animations = toPlay;
	linePositions = toMove;
}

void CharacterController::setCurrentSpot(int x)
{
	currentspot = x;
	
}

void CharacterController::setStopSpot(int x)
{
	stopSpot = x;
}

void CharacterController::queueAnimation(int x)
{
	currentAnimation = x;
}

void CharacterController::updateAnimation(float deltaTime)
{
	float nextT = animations[currentAnimation]->getAddedT(deltaTime);
	bool doneAnimation = false;
	if (nextT >= 1.f) {
		doneAnimation = true;
	}
	if (inTransition) {
		transitionT += transitionTAdder;
	}
	if (doneAnimation) {
		inTransition = false;
		transitionT = 0.f;
		if (queuedAnimation >= 0) {
			startTransition = animations[currentAnimation]->getMeshAtFrame(animations[currentAnimation]->getCurrentFrame());//probably should be next frame
			endTransition = animations[queuedAnimation]->getMeshAtFrame(0);//start of next animation
			transitionTAdder = (animations[currentAnimation]->getAddedT(deltaTime) + animations[queuedAnimation]->getAddedT(deltaTime)) / 2;
		}
	}
	else
	{
		if (!inTransition) {
			animations[currentAnimation]->update(owner, deltaTime, stopAnimation);

		}
		else
		{
			owner->Get<CMorphAnimator>().setMeshAndTime(startTransition, endTransition, transitionT);
		}
	}
	
	
}

void CharacterController::updatePosition()
{
}

void CharacterController::continueAnimation(bool x)
{
	stopAnimation = !x;
}
