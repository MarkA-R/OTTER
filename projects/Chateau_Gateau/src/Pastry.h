#pragma once
#include "bakeryUtils.h"
class Pastry
{
public:
	float getCookedSeconds();
	void addCookedSeconds(float i);
	void setCookedSeconds(float i);
	void setType(bakeryUtils::pastryType t);
	void setFilling(bakeryUtils::fillType t);
	void setTopping(bakeryUtils::toppingType t);
	bakeryUtils::pastryType getPastryType();
	bakeryUtils::fillType getPastryFilling();
	bakeryUtils::toppingType getPastryTopping();
	float getNextCookTime();
	float getCurrentCookedTime();
	void updateType();
	void setInOven(bool in);
	bool isInOven();
protected:
	float secondsCooked = 0;
	bakeryUtils::fillType filling = bakeryUtils::fillType::NONE;
	bakeryUtils::toppingType topping = bakeryUtils::toppingType::NONE;
	bakeryUtils::pastryType type = bakeryUtils::pastryType::DOUGH;
	bool inOven = false;
};

