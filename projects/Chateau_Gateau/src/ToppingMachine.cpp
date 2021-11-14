#include "ToppingMachine.h"
#include <iostream>

void ToppingMachine::setTransform(Transform& a, Transform& b)
{
	topTransform0 = a;
	topTransform1 = b;

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
	int prevSelected = selectedTopping;
	int adder = 0;
	if (x < 0) {
		adder = 2;
	}

	if (x > 0) {
		adder = 1;
	}
	selectedTopping = (prevSelected + adder) % 3;
	std::cout << selectedTopping << " " << adder << std::endl;
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
}

void ToppingMachine::putInTopping(Entity* e)
{
	inTopping = e;
	//e->transform.m_pos = fillingTransform.m_pos;
	//e->transform.SetParent(nullptr);
}

int ToppingMachine::getSelectedNumber()
{
	return selectedTopping;
}
