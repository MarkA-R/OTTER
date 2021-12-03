#include "Order.h"
#include <ctime>

float Order::getWorkTime()
{
	return workTime;
}

void Order::setStarted(bool b)
{
	hasStarted = b;
}

bool Order::orderStarted()
{
	return hasStarted;
}

void Order::createOrder(int difficulty)
{
	
	isPastrySatisfied = false;
	isDrinkSatisfied = true;
	if (difficulty > 4) {
		difficulty = 4;
	}
	
	int random = (rand() % (difficulty)) + 1;
	if (random > 4) {
		random = 4;
	}
	if (random < 1) {
		random = 1;
	}

	int fillingRand = (rand() % 3) + 1;
	int toppingRand = (rand() % 3) + 1;
	int drinkRand = (rand() % 3) + 1;
	float addedDrinkSecs = 0;
	//std::cout << fillingRand << " " << toppingRand << std::endl;
	int usingPastryInt = random;
	if (difficulty == 1) {
		random = 1;
	}
	drink = bakeryUtils::drinkType::NONE;
	type =bakeryUtils::pastryType((random));
	if (difficulty > 2) {
		topping = bakeryUtils::toppingType(toppingRand);
	}
	else
	{
		topping = bakeryUtils::toppingType::NONE;
	}
	if (difficulty > 1) {
		filling = bakeryUtils::fillType(fillingRand);
	}
	else
	{
		filling = bakeryUtils::fillType::NONE;
	}

	
	if (difficulty == 4 && bakeryUtils::getRoundsLasted()  >= 6) {
		drink = bakeryUtils::drinkType(drinkRand);
		addedDrinkSecs = bakeryUtils::getDrinkFillAmount();
		
		isDrinkSatisfied = false;
	}
	std::cout << "DRINKSEC " << addedDrinkSecs << std::endl;
	float orderSeconds = (60 / (difficulty)) + (bakeryUtils::returnBakeTime(type) + addedDrinkSecs);//60
	std::cout << "ORDERSEC " << orderSeconds << std::endl;
	workTime = orderSeconds;
	hasStarted = false;
	startTime = -1;
}

void Order::translate(bakeryUtils::toppingType topping, bakeryUtils::fillType filling, bakeryUtils::pastryType type, bakeryUtils::drinkType drink)
{
	if (topping == bakeryUtils::toppingType::NONE)
	{
		s_topping = "NONE";
	}
	else if (topping == bakeryUtils::toppingType::PECAN)
	{
		s_topping = "PECAN";
	}
	else if (topping == bakeryUtils::toppingType::STRAWBERRY)
	{
		s_topping = "STRAWBERRY";
	}
	else if (topping == bakeryUtils::toppingType::SPRINKLE)
	{
		s_topping = "SPRINKLE";
	}
		
	

	if (filling == bakeryUtils::fillType::NONE)
	{
		s_filling = "NONE";
	}
	else if (filling == bakeryUtils::fillType::CHOCOLATE)
	{
		s_filling = "CHOCOLATE";
	}
	else if (filling == bakeryUtils::fillType::CUSTARD)
	{
		s_filling = "CUSTARD";
	}
	else if (filling == bakeryUtils::fillType::JAM)
	{
		s_filling = "JAM";
	}

	if (drink == bakeryUtils::drinkType::NONE)
	{
		s_drink = "NONE";
	}
	else if (drink == bakeryUtils::drinkType::COFFEE)
	{
		s_drink = "COFFEE";
	}
	else if (drink == bakeryUtils::drinkType::MILKSHAKE)
	{
		s_drink = "MILKSHAKE";
	}
	else if (drink == bakeryUtils::drinkType::TEA)
	{
		s_drink = "TEA";
	}

	if(type == bakeryUtils::pastryType::CROISSANT)
	{
		s_type = "CROISSANT";
	}
	else if (type == bakeryUtils::pastryType::COOKIE)
	{
		s_type = "COOKIE";
	}
	else if (type == bakeryUtils::pastryType::CUPCAKE)
	{
		s_type = "CUPCAKE";
	}
	else if (type == bakeryUtils::pastryType::CAKE)
	{
		s_type = "CAKE";
	}
	
}

void Order::startOrder()
{
	setStarted(true);

	translate(topping,filling,type,drink);
	std::cout << "I want a " << s_type << " filled with " << s_filling << " and topped with " << s_topping << " and a " << s_drink << "." << std::endl;
	if (startTime < 0) {
		startTime = bakeryUtils::getTime(); //bakeryStats.getGameTime();
	}
	
	maxEndTime = startTime + workTime;
	//std::cout << "START" << startTime << std::endl;
}

void Order::setOver(bool x)
{
	//hasStarted = x;
}



bool Order::validateOrder(Pastry p)
{
	if ((int)p.getPastryType() == int(type))
	{
		if ((int)p.getPastryFilling() == (int)filling)
		{
			if ((int)p.getPastryTopping() == (int)topping)
			{
				
				return true;
			}
		}
	}

	return false;
}

bool Order::validateDrink(Drink d)
{
	if ((int)d.getDrinkType() == int(drink))
	{
		
		return true;
	}
	return false;
}

bool Order::returnSatisfied()
{
	
	if (isPastrySatisfied && isDrinkSatisfied) {
		return true;
	}
	return false;
}

bool Order::isOnTime()
{
	if (hasStarted)
	{
		//std::cout << timePassed << " " << maxEndTime;
		if (bakeryUtils::getTime() <= maxEndTime)
		{
			return true;
		}
	}
	else
	{
		return true;
	}

	return false;
}

void Order::setPastryValidated(bool x)
{
	isPastrySatisfied = x;
}

void Order::setDrinkValidated(bool x)
{
	isDrinkSatisfied = x;
}

Pastry Order::toPastry()
{
	Pastry p = Pastry();
	p.setFilling(filling);
	p.setType(type);
	p.setTopping(topping);
	return p;
}

Drink Order::toDrink()
{
	return Drink();
}

void Order::update(float deltaTime)
{
	timePassed += deltaTime;
}