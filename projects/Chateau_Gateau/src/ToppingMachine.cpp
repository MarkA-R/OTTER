#include "ToppingMachine.h"

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
