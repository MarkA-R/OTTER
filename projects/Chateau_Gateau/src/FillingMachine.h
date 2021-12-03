#pragma once
#include "Pastry.h"
#include "NOU/Entity.h"
#include "NOU/CMeshRenderer.h"
#include "bakeryUtils.h"
#include "MaterialCreator.h"

using namespace nou;

class FillingMachine
{
protected:
	Entity* inFilling;
	Entity* fillingPlane;
	Transform fillingTransform;
	float fillingT;
	std::vector<Mesh*> frames = std::vector<Mesh*>();
	int currentFrame;
	int nextFrame;
	int selectedFilling = 0;
	MaterialCreator* planeDesigns[3];
public:
	void setFrames(std::vector<Mesh*>);
	void setTransform(Transform&);
	void setCurrentFrame(int);
	void setNextFrame(int);
	Mesh* getFrame(int);
	int getCurrentFrame();
	int getNextFrame();
	void setT(float);
	float getT();
	bakeryUtils::fillType getFilling();
	void addFillNum(int);
	void setFillNum(int);
	void setFillingPlane(Entity*);
	Entity* getFillingPlane();
	void updatePlane();
	void setup(MaterialCreator*, MaterialCreator*, MaterialCreator*);
	bool isFillingFull();
	Entity* getFromFilling();
	void removeFromFilling();
	void putInFilling(Entity*);
	glm::vec3 getFillingPosition();
};

