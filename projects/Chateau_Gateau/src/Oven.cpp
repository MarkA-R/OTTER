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

void Oven::setMaterials(std::vector<MaterialCreator*> a)
{
	materials = a;
}

void Oven::update(float dt)
{
	secondCounter += dt;
	if (secondCounter >= updateTime) {
		

		for (int i = 0; i < std::size(inOven); i++) {
			if (inOven[i] != nullptr) {
				//std::cout << "COUNTED" << std::endl;
				Pastry* currentPastry = &inOven[i]->Get<Pastry>();
				float timeLeft =
					1 - ((currentPastry->getCookedSeconds() - currentPastry->getCurrentCookedTime())
						/
						(currentPastry->getNextCookTime() - currentPastry->getCurrentCookedTime()));
				if (currentPastry->getPastryType() != bakeryUtils::pastryType::BURNT) {
					ovenTimers[i]->setFill(timeLeft);
					ovenTimers[i]->updateArrow();
				}
				else
				{
					ovenTimers[i]->setFill(0);
					ovenTimers[i]->updateArrow();
				}
				
				currentPastry->addCookedSeconds(updateTime);
				//std::cout << currentPastry->getCookedSeconds() << std::endl;
				currentPastry->updateType();
				int pastryIndex = ((int)currentPastry->getPastryType()) + 1;//cause nothing tile
				ovenTimers[i]->setMaterial(*materials[pastryIndex]);


			}
		}
		secondCounter -= updateTime;
	}
}

bool Oven::canAdd(Entity* e, int slot)
{
	if (e->Has<Pastry>()) {
		if (slot >= 0 && slot < std::size(inOven)) {
			if (inOven[slot] == nullptr) {
				//e->transform.SetParent(nullptr);
				//e->transform.m_pos = insideOven.m_pos;
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
		ovenTimers[slot]->setFill(0);
		ovenTimers[slot]->updateArrow();
		ovenTimers[slot]->setMaterial(*materials[0]);

	}
}

bool Oven::isSlotFull(int slot)
{
	if(inOven[slot] == nullptr) {
		return false;
	}
	return true;
}

int Oven::getFirstEmpty() {
	for (int i = 0; i < std::size(inOven); i++) {
		if (!isSlotFull(i)) {
			return i;
		}
	}
	return -1;
}

Transform* Oven::getInsideOven()
{
	return &insideOven;
}


