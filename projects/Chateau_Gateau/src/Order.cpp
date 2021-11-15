#include "Order.h"

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
	if (difficulty > 4) {
		difficulty = 4;
	}
	
	int random = (rand() % (difficulty)) + 1;
	int fillingRand = (rand() % 3) + 1;
	int toppingRand = (rand() % 3) + 1;
	//std::cout << fillingRand << " " << toppingRand << std::endl;
	int usingPastryInt = random;
	if (difficulty == 1) {
		random = 0;
	}
	
	type =bakeryUtils::pastryType((random) + 1);
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
	
	
	float orderSeconds = (60 / (difficulty)) + bakeryUtils::returnBakeTime(type);//60
	std::cout << "ORDERSEC " << orderSeconds << std::endl;
	workTime = orderSeconds;
	hasStarted = false;
}

void Order::translate(bakeryUtils::toppingType topping, bakeryUtils::fillType filling, bakeryUtils::pastryType type)
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
	translate(topping,filling,type);
	std::cout << "I want a " << s_type << " filled with " << s_filling << " and topped with " << s_topping << "." << std::endl;
	startTime = bakeryUtils::getTime(); //bakeryStats.getGameTime();
	maxEndTime = startTime + workTime;
	//std::cout << "START" << startTime << std::endl;
}

void Order::setOver(bool)
{
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

bool Order::returnSatisfied()
{
	return isSatisfied;
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

Pastry Order::toPastry()
{
	Pastry p = Pastry();
	p.setFilling(filling);
	p.setType(type);
	p.setTopping(topping);
	return p;
}

void Order::update(float deltaTime)
{
	timePassed += deltaTime;
}