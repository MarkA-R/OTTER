#pragma once
#include "Pastry.h"
#include "NOU/Entity.h"
#include "NOU/CMeshRenderer.h"
#include "bakeryUtils.h"
#include "MaterialCreator.h"
class ToppingMachine
{
protected:
	Entity* inTopping;
	Entity* toppingPlane;
	Transform topTransform0;
	Transform topTransform1;
	Transform particleTransform;
	float toppingT;
	int selectedTopping = 0;
	MaterialCreator* planeDesigns[3];
	Material* particleDesigns[3];
	glm::vec3 particleColours[3];
public:
	void setTransform(Transform&, Transform&);
	void setParticleTransform(Transform&);
	void setT(float);
	float getT();
	bakeryUtils::toppingType getTopping();
	void addTopNum(int);
	void setTopPlane(Entity*);
	Entity* getToppingPlane();
	void updatePlane();
	void setup(MaterialCreator*, MaterialCreator*, MaterialCreator*);
	void setupParticles(Material*, Material*, Material*);
	Material* getParticleMaterial(int);
	glm::vec3 getParticleColour(int);
	void setupParticleColours(glm::vec3, glm::vec3, glm::vec3);
	bool isToppingFull();
	Entity* getFromTopping();
	void removeFromTopping();
	void putInTopping(Entity*);
	int getSelectedNumber();
};

