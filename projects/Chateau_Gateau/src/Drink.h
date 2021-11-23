#pragma once
#include "bakeryUtils.h"
class Drink
{
protected:
	bakeryUtils::drinkType type = bakeryUtils::drinkType::NONE;
public:
	void setDrinkType(bakeryUtils::drinkType);
	bakeryUtils::drinkType getDrinkType();
};

