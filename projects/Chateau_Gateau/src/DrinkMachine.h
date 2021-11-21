#pragma once
#include "Pastry.h"
#include "NOU/Entity.h"
#include "NOU/CMeshRenderer.h"
#include "bakeryUtils.h"
#include "MaterialCreator.h"
using namespace nou;

class DrinkMachine
{
protected:
	Entity* inDrink;
	Entity* drinkPlane;
	Transform drinkTransformIn;
	Transform drinkTransformOut;
	float drinkT = 0.f;//this is for the morph target animation
	int selectedDrink = 0;
	MaterialCreator* planeDesigns[3];
	std::vector<Mesh*> frames = std::vector<Mesh*>();
	int currentFrame;
	int nextFrame;
	float drinkFill = 0.f;//this is for making a new drink
public:
	void setTransform(Transform&, Transform&);
	void setT(float);
	float getT();
	bakeryUtils::drinkType getDrink();
	void addDrinkNum(int);
	void setDrinkPlane(Entity*);
	Entity* getDrinkPlane();
	void updatePlane();
	void setup(MaterialCreator*, MaterialCreator*, MaterialCreator*);
	void moveDrink(float t);
	bool isDrinkFull();
	Entity* getFromDrink();
	void removeFromDrink();
	void addFill(float x);
	float getFill();
};

