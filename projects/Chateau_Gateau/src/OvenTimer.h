#pragma once
#include "NOU/Entity.h"
#include "NOU/GLTFLoader.h"
#include "MaterialCreator.h"
using namespace nou;
class OvenTimer
{
public:
	OvenTimer(MaterialCreator& t, MaterialCreator& a, MaterialCreator& c, Transform& pos, float multiplier = 0.3f);
	//Timer(MaterialCreator right, MaterialCreator left, Transform pos);
	void addFill(float);
	float getFill();
	void setFill(float);
	void setMaterial(MaterialCreator&);
	Entity* getTile();
	Entity* getArrow();
	Entity* getCircle();
	void updateArrow();
	Transform& getTransform();
	void setTransform(Transform&);
	void setPosition(glm::vec3);
	void dontShow(int);//0 for tile, 1 for arrow, 2 for circle

protected:
	float fill = 0;
	std::unique_ptr<Entity> tile;
	std::unique_ptr<Entity> arrow;
	std::unique_ptr<Entity> circle;
	Transform position;
};

