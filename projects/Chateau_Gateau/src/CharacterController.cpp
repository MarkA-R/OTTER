#include "CharacterController.h"
#include <iostream>

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
	queuedAnimation = x;
}

void CharacterController::updateAnimation(float deltaTime)
{
	float nextT = animations[currentAnimation]->getAddedT(deltaTime);
	
	bool doneAnimation = false;
	if (animations[currentAnimation]->getT() + nextT >= 1.f) {
		doneAnimation = true;
	}
	
	if (inTransition) {
		transitionT += transitionTAdder;
	}
	
	
	//std::cout << "CURRENT: " <<  animations[currentAnimation]->getT() << std::endl;
	//std::cout << "QUEUED: " <<  animations[queuedAnimation]->getT() << std::endl;
	
	if (doneAnimation) {
		
		
		if (queuedAnimation >= 0 && queuedAnimation != currentAnimation) {
			startTransition = animations[currentAnimation]->getMeshAtFrame(animations[currentAnimation]->getNextFrame());//probably should be next frame
			endTransition = animations[queuedAnimation]->getMeshAtFrame(0);//start of next animation
			
			transitionTAdder = (animations[currentAnimation]->getAddedT(deltaTime) + animations[queuedAnimation]->getAddedT(deltaTime)) / 2;
			animations[currentAnimation]->setCurrentFrame(0);
			animations[currentAnimation]->setT(0);
			animations[queuedAnimation]->setCurrentFrame(0);
			animations[queuedAnimation]->setT(0);
			currentAnimation = queuedAnimation;
			queuedAnimation = -1;
			if (owner->Has<CMorphAnimator>()) {
				owner->Get<CMorphAnimator>().SetFrames(animations[currentAnimation]->getFrames());
			}
			transitionT = 0.f;
			inTransition = true;
			
		}
	}
	
	

		if (!inTransition) {
			animations[currentAnimation]->update(owner, deltaTime, stopAnimation);

		}
		else
		{
			owner->Get<CMorphAnimator>().setMeshAndTime(startTransition, endTransition, transitionT);
			if (transitionT >= 1) {
				inTransition = false;

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
