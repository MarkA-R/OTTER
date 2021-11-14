#include "ToppingMachine.h"
#include <iostream>

// Templated LERP function
template<typename T>
T Lerp(const T& p0, const T& p1, float t)
{
	return (1.0f - t) * p0 + t * p1;
}

void ToppingMachine::setTransform(Transform& l, Transform& r)
{
	topTransformL = l;
	topTransformR = r;

}

void ToppingMachine::setParticleTransform(Transform& x)
{
	particleTransform = x;
}

void ToppingMachine::setT(float x)
{
	toppingT = x;
}

float ToppingMachine::getT()
{
	return toppingT;
}

bakeryUtils::toppingType ToppingMachine::getTopping()
{
	return bakeryUtils::toppingType((selectedTopping)+1);//+1 for NONE
}

void ToppingMachine::addTopNum(int x)
{
	//int prevSelected = selectedTopping;
	int prevSelected = selectedTopping;
	int adder = 0;
	if (x < 0) {
		adder = 2;
	}

	if (x > 0) {
		adder = 1;
	}
	selectedTopping = (prevSelected + adder) % 3;
	//selectedTopping += x;
	//std::cout << x << " " << adder << std::endl;
	//std::cout << selectedTopping << std::endl;
}

void ToppingMachine::setTopPlane(Entity* e)
{
	toppingPlane = e;
}

Entity* ToppingMachine::getToppingPlane()
{
	return toppingPlane;
}

void ToppingMachine::updatePlane()
{
	toppingPlane->Get<CMeshRenderer>().SetMaterial(*planeDesigns[selectedTopping]->getMaterial());
}

void ToppingMachine::setup(MaterialCreator* pecan, MaterialCreator* sprinkle, MaterialCreator* strawberry)
{
	planeDesigns[0] = pecan;
	planeDesigns[1] = sprinkle;
	planeDesigns[2] = strawberry;
}

void ToppingMachine::setupParticles(Material* pecan, Material* sprinkle, Material* strawberry)
{
	particleDesigns[0] = pecan;
	particleDesigns[1] = sprinkle;
	particleDesigns[2] = strawberry;

}

void ToppingMachine::moveTopping(float t)
{
	inTopping->transform.m_pos = Lerp(topTransformL.m_pos, topTransformR.m_pos, t);
}

Material* ToppingMachine::getParticleMaterial(int i)
{
	return particleDesigns[i];
}

glm::vec3 ToppingMachine::getParticleColour(int i)
{
	return particleColours[i];
}


void ToppingMachine::setupParticleColours(glm::vec3 pecan, glm::vec3 sprinkle, glm::vec3 strawberry)
{
	particleColours[0] = pecan;
	particleColours[1] = sprinkle;
	particleColours[2] = strawberry;
}

bool ToppingMachine::isToppingFull()
{
	if (inTopping == nullptr) {
		return false;
	}
	return true;
}

Entity* ToppingMachine::getFromTopping()
{
	return inTopping;
}

void ToppingMachine::removeFromTopping()
{
	inTopping = nullptr;
	totalDistanceTravelled = 0.f;
}

void ToppingMachine::putInTopping(Entity* e)
{
	inTopping = e;
	e->transform.m_pos = Lerp(topTransformL.m_pos, topTransformR.m_pos, toppingT);
	e->transform.SetParent(nullptr);
}

int ToppingMachine::getSelectedNumber()
{
	return selectedTopping;
}

void ToppingMachine::addDistance(float t)
{
	totalDistanceTravelled += abs(t);
}

float ToppingMachine::getDistance()
{
	return totalDistanceTravelled;
}
