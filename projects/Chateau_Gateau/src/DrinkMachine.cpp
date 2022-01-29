#include "DrinkMachine.h"

// Templated LERP function
template<typename T>
T Lerp(const T& p0, const T& p1, float t)
{
	return (1.0f - t) * p0 + t * p1;
}

void DrinkMachine::setTransform(Transform& in, Transform& out)
{
	drinkTransformIn = in;
	drinkTransformOut = out;

}

void DrinkMachine::setT(float x)
{
	drinkT = x;
}

void DrinkMachine::addToT(float x)
{
	drinkT += x;
	if (drinkT < 0) {
		drinkT = 0;
	}
	if (drinkT > 1) {
		drinkT = 1;
	}
}

float DrinkMachine::getT()
{
	return drinkT;
}

bakeryUtils::drinkType DrinkMachine::getDrink()
{
	return bakeryUtils::drinkType((selectedDrink)+1);//+1 for NONE
}

void DrinkMachine::addDrinkNum(int x)
{
	int prevSelected = selectedDrink;
	int adder = 0;
	if (x < 0) {
		adder = 2;
	}

	if (x > 0) {
		adder = 1;
	}
	selectedDrink = (prevSelected + adder) % 3;
	//std::cout << "SELECTED DRINK" << selectedDrink << std::endl;
}

void DrinkMachine::setDrinkNum(int x)
{
	selectedDrink = x;
}

void DrinkMachine::setDrinkPlane(Entity* plane)
{
	drinkPlane = plane;
}

Entity* DrinkMachine::getDrinkPlane()
{
	return drinkPlane;
}

void DrinkMachine::updatePlane()
{
	drinkPlane->Get<CMeshRenderer>().SetMaterial(*planeDesigns[selectedDrink]->getMaterial());
}

void DrinkMachine::setup(MaterialCreator* coffee, MaterialCreator* milkshake, MaterialCreator* tea, FillBar* fill)
{
	drinkFillBar = fill;
	planeDesigns[0] = coffee;
	planeDesigns[1] = milkshake;
	planeDesigns[2] = tea;
}


void DrinkMachine::moveDrink(float t)
{
	if (t > 1) {
		t = 1;
	}
	if (t < 0) {
		t = 0;
	}
	inDrink->transform.m_pos = Lerp(drinkTransformIn.m_pos, drinkTransformOut.m_pos, t);
}

bool DrinkMachine::isDrinkFull()
{
	if (inDrink != nullptr) {
		return true;
	}
	return false;
}

void DrinkMachine::createDrink()
{
	inDrink = Entity::Allocate();
	inDrink.get()->transform.m_pos = drinkTransformIn.m_pos;
	inDrink.get()->transform.m_rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0, 0, 1));
	inDrink.get()->transform.m_scale = glm::vec3(1);
	inDrink.get()->Add<Transparency>(*inDrink.get());
}

Entity* DrinkMachine::getFromDrink()
{
	return inDrink.get();
}

Entity* DrinkMachine::releaseFromDrink()
{
	return inDrink.release();
}

void DrinkMachine::removeFromDrink()
{
	//inDrink.release();
	inDrink = nullptr;
	//drinkFill = 0;
}

void DrinkMachine::addFill(float x)
{
	drinkFill += x;
	if (drinkFill < 0) {
		drinkFill = 0;
	}
}

float DrinkMachine::getFill()
{
	return drinkFill;
}

void DrinkMachine::setFill(float x)
{
	drinkFill = x;
}

FillBar* DrinkMachine::getFillBar()
{
	return drinkFillBar;
}
