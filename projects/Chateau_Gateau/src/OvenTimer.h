#pragma once
#include "NOU/Entity.h"
#include "NOU/GLTFLoader.h"
#include "MaterialCreator.h"
using namespace nou;
class OvenTimer
{
public:
	OvenTimer(MaterialCreator& t, MaterialCreator& a, MaterialCreator& c, Transform& pos);
	//Timer(MaterialCreator right, MaterialCreator left, Transform pos);
	void addFill(float);
	float getFill();
	void setFill(float);
	void setMaterial(MaterialCreator);
	Entity* getTile();
	Entity* getArrow();
	Entity* getCircle();
	void updateArrow();

protected:
	float fill = 0;
	std::unique_ptr<Entity> tile;
	std::unique_ptr<Entity> arrow;
	std::unique_ptr<Entity> circle;
	Transform position;
};

