#pragma once
#include "Pastry.h"
#include "OvenTimer.h"
class Oven
{
protected:
	Entity* inOven[4] = {};
	OvenTimer* ovenTimers[4] = {};
	float secondCounter = 0;
	Transform insideOven;
	float updateTime = 0.1;
	std::vector<MaterialCreator*> materials;//lowest is burnt, highest is cake
public:
	void setup(OvenTimer* a, OvenTimer* b, OvenTimer* c, OvenTimer* d, Transform* t);
	void setMaterials(std::vector<MaterialCreator*> a);
	void update(float dt);
	bool canAdd(Entity* e, int slot);
	bool canRemove(int slot);
	Entity& getEntity(int slot);
	void removeFromSlot(int slot);
	bool isSlotFull(int slot);
	int getFirstEmpty();
};

