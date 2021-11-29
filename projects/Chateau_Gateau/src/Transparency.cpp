#include "Transparency.h"
#include "NOU/CMeshRenderer.h"
#include <iostream>
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
	
	if (transparencyT >= 0) {
		transparencyT += deltaTime / timeToLERP;
	}
	if (transparencyT > 1) {
		currentTransparency = wantedTransparency;
		transparencyT = -1;
		wantedTransparency = -1;
		if (newParent != &owner->transform) {
			owner->transform.SetParent(newParent);
			newParent = nullptr;
		}
		if (nextPosition.x > -999) {
			owner->transform.m_pos = (nextPosition);
			nextPosition = glm::vec3(-999);
		}
		
		if (inverseCopy != nullptr) {
			inverseCopy->~Entity();
		}
		
		
	}
	if (wantedTransparency >= 0) {
		
		currentTransparency = Lerp(beginingTransparency, wantedTransparency, transparencyT);
	}
	
	
}

float Transparency::getTransparency()
{
	return currentTransparency;
}

void Transparency::setWantedTransparency(float x , float time)
{
	beginingTransparency = currentTransparency;
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
	//transparencyT = 0.f;
}

void Transparency::setInverseCopy(Transform* newT, MaterialCreator* mat, bool setParent)
{
	inverseCopy = Entity::Allocate().get();
	inverseCopy->Add<Transparency>(*inverseCopy);
	inverseCopy->Add<CMeshRenderer>(*inverseCopy, *mat->getMesh(), *mat->getMaterial());

	inverseCopy->Get<Transparency>().setTransparency(1 - currentTransparency);
	inverseCopy->Get<Transparency>().setWantedTransparency(1 - wantedTransparency);
	inverseCopy->Get<Transparency>().setTime(timeToLERP);
	
	inverseCopy->transform.m_rotation = owner->transform.m_rotation;
	inverseCopy->transform.m_scale = owner->transform.m_scale;
	inverseCopy->transform.m_pos = newT->m_pos;
	if (setParent) {
		inverseCopy->transform.SetParent(newT);
	}
}

Entity* Transparency::getInverseCopy()
{
	return inverseCopy;
}
