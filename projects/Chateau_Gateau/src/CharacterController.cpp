#include "CharacterController.h"
#include <iostream>

template<typename T>
T Catmull(const T& p0, const T& p1, const T& p2, const T& p3, float t)
{

	return 0.5f * (2.f * p1 + t * (-p0 + p2)
		+ t * t * (2.f * p0 - 5.f * p1 + 4.f * p2 - p3)
		+ t * t * t * (-p0 + 3.f * p1 - 3.f * p2 + p3));
}

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

void CharacterController::updatePosition(float deltaTime, float seconds)
{

	lineT += (deltaTime / seconds);
	bool isStopped = false;
	if (currentspot >= stopSpot && lineT >= 1.f) {
		isStopped = true;
		lineT = 1.f;
	}
	if (!isStopped) {
		if (lineT >= 1) {
			lineT -= 1;
			currentspot++;
			nextSpot++;
		}
	}
	
	
	if (nextSpot >= linePositions.size()) {
		nextSpot = linePositions.size() - 1;
		currentspot = linePositions.size() - 2;
		lineT = 1.0f;
		isStopped = true;
	}
	int p0 = 0;
	int p3 = linePositions.size();
	if (currentspot > 0) {
		p0 = currentspot -1;
	}
	if (nextSpot < linePositions.size() - 1) {
		p3 = nextSpot + 1;
	}
	if (p3 >= linePositions.size()) {
		p3 = linePositions.size() - 1;
	}
	if (p0 < 0) {
		p0 = 0;
	}

	if (!isStopped) {
		bool keepComputing = true;

		glm::vec3 oldPos = owner->transform.m_pos;
		glm::vec3 newPos = Catmull(linePositions[p0], linePositions[currentspot], linePositions[nextSpot],
			linePositions[p3], lineT);
		owner->transform.m_pos = newPos;


		//https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/lookat-function
		glm::vec3 newDir = (glm::normalize(newPos - oldPos));
		
		glm::vec3 subtractedVectors = (oldPos - newPos);
		if (subtractedVectors.x == 0.f && subtractedVectors.y == 0.f && subtractedVectors.z == 0.f) {
			keepComputing = false;
		}
		if (keepComputing) {
			glm::vec3 up = glm::vec3(0, 1, 0);
			glm::vec3 right = glm::cross(up, newDir);
			glm::mat3 rotationMatrix;
			rotationMatrix[0][0] = right.x;
			rotationMatrix[0][1] = right.y;
			rotationMatrix[0][2] = right.z;
			rotationMatrix[1][0] = up.x;
			rotationMatrix[1][1] = up.y;
			rotationMatrix[1][2] = up.z;
			rotationMatrix[2][0] = newDir.x;
			rotationMatrix[2][1] = newDir.y;
			rotationMatrix[2][2] = newDir.z;
			
			owner->transform.m_rotation = rotationMatrix;
		}
		
	}
	

	
}

void CharacterController::continueAnimation(bool x)
{
	stopAnimation = !x;
}
