#pragma once
#include "bakeryUtils.h"
#include "Pastry.h"
#include "Drink.h"
#include <iostream>
class Order
{
public:
	bakeryUtils::toppingType topping;
	bakeryUtils::fillType filling;
	bakeryUtils::pastryType type;
	bakeryUtils::drinkType drink;
	float startTime = -1;
	float workTime = 0;
	float maxEndTime = 0;

	std::string s_topping;
	std::string s_filling;
	std::string s_type;
	std::string s_drink;


	float timePassed;

	float getWorkTime();
	void setStarted(bool b);
	bool orderStarted();
	void createOrder(int difficulty);
	void translate(bakeryUtils::toppingType topping, bakeryUtils::fillType filling, bakeryUtils::pastryType type, bakeryUtils::drinkType drink);
	void startOrder();
	void setOver(bool);

	bool validateOrder(Pastry p);

	bool validateDrink(Drink d);

	bool returnSatisfied();

	bool isOnTime();

	void setPastryValidated(bool x);
	void setDrinkValidated(bool x);

	Pastry toPastry();

	Drink toDrink();

	void update(float deltaTime);
protected:
	bool isSatisfied = false;
	bool hasStarted = false;
	bool isPastrySatisfied = false;
	bool isDrinkSatisfied = true;
	
};