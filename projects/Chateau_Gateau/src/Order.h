#pragma once
#include "bakeryUtils.h"
#include "Pastry.h"
#include <iostream>
class Order
{
public:
	bakeryUtils::toppingType topping;
	bakeryUtils::fillType filling;
	bakeryUtils::pastryType type;
	float startTime = 0;
	float workTime = 0;
	float maxEndTime = 0;
	
	std::string s_topping;
	std::string s_filling;
	std::string s_type;

	float timePassed;

	float getWorkTime();
	void setStarted(bool b);
	bool orderStarted();
	void createOrder(int difficulty);
	void translate(bakeryUtils::toppingType topping, bakeryUtils::fillType filling, bakeryUtils::pastryType type);
	void startOrder();
	bool validateOrder(Pastry p);

	bool returnSatisfied();
	
	bool isOnTime();

	Pastry toPastry();

	void update(float deltaTime);
protected:
	bool isSatisfied = false;
	bool hasStarted = false;
};

