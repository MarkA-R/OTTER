#include "Oven.h"
#include <iostream>


void Oven::setup(OvenTimer* a, OvenTimer* b, OvenTimer* c, OvenTimer* d, Transform* t)
{
	ovenTimers[0] = a;
	ovenTimers[1] = b;
	ovenTimers[2] = c;
	ovenTimers[3] = d;
	insideOven = *t;
}

void Oven::update(float dt)
{
	secondCounter += dt;
	if (secondCounter >= 1) {
		

		for (int i = 0; i < std::size(inOven); i++) {
			if (inOven[i] != nullptr) {
				std::cout << "COUNTED" << std::endl;
				Pastry* currentPastry = &inOven[i]->Get<Pastry>();
				float timeLeft =
					1 - ((currentPastry->getCookedSeconds() - currentPastry->getCurrentCookedTime())
						/
						(currentPastry->getNextCookTime() - currentPastry->getCurrentCookedTime()));
				ovenTimers[i]->setFill(timeLeft);
				ovenTimers[i]->updateArrow();
				currentPastry->addCookedSeconds(1);
				currentPastry->updateType();

			}
		}
		secondCounter -= 1;
	}
}

bool Oven::canAdd(Entity* e, int slot)
{
	if (e->Has<Pastry>()) {
		if (slot >= 0 && slot < std::size(inOven)) {
			if (inOven[slot] == nullptr) {
				e->transform.SetParent(nullptr);
				e->transform.m_pos = insideOven.m_pos;
				inOven[slot] = e;
				return true;
			}
		}
		
	}
	return false;
}

bool Oven::canRemove(int slot)
{
	if (inOven[slot] != nullptr) {
		return true;
	}
	return false;
}

Entity& Oven::getEntity(int slot)
{
	return *inOven[slot];
}

void Oven::removeFromSlot(int slot)
{
	if (slot >= 0 && slot < std::size(inOven)) {
		inOven[slot] = nullptr;
	}
}


