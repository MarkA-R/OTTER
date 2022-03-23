#include "Transparency.h"

// Templated LERP function
template<typename T>
T Lerp(const T& p0, const T& p1, float t)
{
	return (1.0f - t) * p0 + t * p1;
}
Transparency::Transparency(Entity& e)
{
	owner = &e;
}
void Transparency::setTransparency(float x)
{
	currentTransparency = x;
	beginingTransparency = currentTransparency;
}

void Transparency::updateTransparency(float deltaTime)
{
	
	if (transparencyT >= 0.f) {
		transparencyT += deltaTime / timeToLERP;
	}
	if (output) {
		std::cout << "T: " << transparencyT << " P:" << owner->transform.m_pos.x << " "
			<< owner->transform.m_pos.y << " "
			<< owner->transform.m_pos.z << " " << std::endl;
	}
	
	if (transparencyT > 1) {
	
			currentTransparency = wantedTransparency;
			transparencyT = -1;
			wantedTransparency = -1;
			glm::vec3 goingPos = nextPosition;
			if (newTransPos != nullptr) {
				//if (newTransPos->m_pos.x > -999) {
					goingPos = newTransPos->m_pos;
					
				//}
				
			}
		if (goingPos.x > -999) {
			owner->transform.m_pos = (goingPos);
			if (newParent != &owner->transform) {
				owner->transform.SetParent(newParent);
				owner->transform.m_pos = (goingPos);
				newParent = nullptr;
			}
			
			goingPos = glm::vec3(-999);
			nextPosition = glm::vec3(-999);
			newTransPos = nullptr;

			if (nextWantedTransparency >= 0) {
				
				
				
				wantedTransparency = nextWantedTransparency;

				beginingTransparency = currentTransparency;
				//currentTransparency = beginingTransparency;
				nextWantedTransparency = -1.f;
				timeToLERP = nextTime;
				transparencyT = 0.f;
				
				copiedScale = glm::vec3(-1);
				if (nextScale.x >= 0) {
					copiedScale = nextScale;
					nextScale = glm::vec3(-1);
				}

			}
			else
			{
				nextWantedTransparency = -1;
				nextScale = glm::vec3(-1);
				
				
			}
		}
		
		
		
		
		
	}
	if (wantedTransparency >= 0) {
		if (copiedScale.x >= 0) {
			owner->transform.m_scale = copiedScale;
		}
		//std::cout << currentTransparency << std::endl;
		currentTransparency = Lerp(beginingTransparency, wantedTransparency, transparencyT);
	}
	
	
}

float Transparency::getTransparency()
{
	return currentTransparency;
}

void Transparency::setWantedTransparency(float x)
{
	beginingTransparency = currentTransparency;
	wantedTransparency = x;
	transparencyT = 0.f;
	//timeToLERP = time;
}

float Transparency::getWantedTransparency()
{
	return wantedTransparency;
}

float Transparency::getTime()
{
	return timeToLERP;
}

void Transparency::setTime(float x)
{
	timeToLERP = x;
}

void Transparency::setNextPosition(glm::vec3 x, Transform* remove, glm::vec3 scale)
{
	nextPosition = x;
	newParent = remove;
	copiedScale = scale;
	//transparencyT = 0.f;
}

void Transparency::setNextTransformPos(Transform* x, Transform* remove, glm::vec3 scale)
{
	newTransPos = x;
	newParent = remove;
	copiedScale = scale;
}

void Transparency::setNextWantedTransparency(float x)
{
	nextWantedTransparency = x;
}

void Transparency::setNextWantedTime(float x)
{
	nextTime = x;
}

void Transparency::setNextWantedScale(glm::vec3 x)
{
	nextScale = x;
}

void Transparency::setOutput(bool x)
{
	output = x;
}

bool Transparency::isFadingIn()
{
	if (wantedTransparency < currentTransparency && transparencyT != 0.f) {
		return true;
	}
	return false;
}

