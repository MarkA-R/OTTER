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
	type = static_cast<bakeryUtils::pastryType>((rand() % difficulty) + 1);
	topping = static_cast<bakeryUtils::toppingType>((rand() % 3) + 1);
	filling = static_cast<bakeryUtils::fillType>((rand() & 3) + 1);
	float orderSeconds = (60 / (difficulty + 1)) + bakeryUtils::returnBakeTime(type);
	workTime = orderSeconds;
	hasStarted = false;
}

void Order::translate(bakeryUtils::toppingType topping, bakeryUtils::fillType filling, bakeryUtils::pastryType type)
{
	switch (topping)
	{
	case bakeryUtils::toppingType::NONE:
		s_topping = "NONE";
		break;
	case bakeryUtils::toppingType::CREAM:
		s_topping = "CREAM";
		break;
	case bakeryUtils::toppingType::SPRINKLE:
		s_topping = "SPRINKLE";
		break;
	case bakeryUtils::toppingType::STRAWBERRY:
		s_topping = "STRAWBERRY";
		break;
	default:
		break;
	}

	switch (filling)
	{
	case bakeryUtils::fillType::NONE:
		s_filling = "NONE";
		break;
	case bakeryUtils::fillType::JAM:
		s_filling = "JAM";
		break;
	case bakeryUtils::fillType::CUSTARD:
		s_filling = "CUSTARD";
		break;
	case bakeryUtils::fillType::CREAM:
		s_filling = "CREAM";
		break;
	default:
		break;
	}

	switch (type)
	{
	case bakeryUtils::pastryType::DOUGH:
		s_type = "DOUGH";
		break;
	case bakeryUtils::pastryType::CROISSANT:
		s_type = "CROISSANT";
		break;
	case bakeryUtils::pastryType::COOKIE:
		s_type = "COOKIE";
		break;
	case bakeryUtils::pastryType::CUPCAKE:
		s_type = "CUPCAKE";
		break;
	case bakeryUtils::pastryType::CAKE:
		s_type = "CAKE";
		break;
	case bakeryUtils::pastryType::BURNT:
		s_type = "BURNT";
		break;
	default:
		break;
	}
}

void Order::startOrder()
{
	setStarted(true);
	std::cout << "I want a " + s_type + " filled with " + s_filling + " and topped with " + s_topping + ".";
	startTime = timePassed; //bakeryStats.getGameTime();
	maxEndTime = startTime + workTime;
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
		if (timePassed <= maxEndTime)
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
	timePassed = deltaTime;
}
