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
}

void Transparency::updateTransparency(float deltaTime)
{
	
	if (transparencyT >= 0) {
		transparencyT += deltaTime / timeToLERP;
	}
	if (transparencyT > 1) {
		transparencyT = -1;
		wantedTransparency = -1;
		if (nextPosition.x > -999) {
			owner->transform.m_pos = (nextPosition);
		}
		if (newParent != &owner->transform) {
			owner->transform.SetParent(newParent);
		}
			
		
		
	}
	if (wantedTransparency >= 0) {
		currentTransparency = Lerp(currentTransparency, wantedTransparency, transparencyT);
	}
	
	
}

float Transparency::getTransparency()
{
	return currentTransparency;
}

void Transparency::setWantedTransparency(float x , float time)
{
	wantedTransparency = x;
	transparencyT = 0.f;
	timeToLERP = time;
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

void Transparency::setNextPosition(glm::vec3 x, Transform* remove)
{
	nextPosition = x;
	newParent = remove;
}
