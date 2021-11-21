#include "DrinkMachine.h"

void DrinkMachine::setTransform(Transform&, Transform&)
{
}

void DrinkMachine::setT(float x)
{
	drinkT = x;
}

float DrinkMachine::getT()
{
	return drinkT;
}

bakeryUtils::drinkType DrinkMachine::getDrink()
{
	return bakeryUtils::drinkType();
}

void DrinkMachine::addDrinkNum(int)
{
}

void DrinkMachine::setDrinkPlane(Entity*)
{
}

Entity* DrinkMachine::getDrinkPlane()
{
	return nullptr;
}

void DrinkMachine::updatePlane()
{
}

void DrinkMachine::setup(MaterialCreator*, MaterialCreator*, MaterialCreator*)
{
}

void DrinkMachine::moveDrink(float t)
{
}

bool DrinkMachine::isDrinkFull()
{
	return false;
}

Entity* DrinkMachine::getFromDrink()
{
	return nullptr;
}

void DrinkMachine::removeFromDrink()
{
}

void DrinkMachine::addFill(float x)
{
}

float DrinkMachine::getFill()
{
	return 0.0f;
}
