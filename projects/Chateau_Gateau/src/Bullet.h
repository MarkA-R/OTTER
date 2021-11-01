#pragma once
#include "glHeaders.h"
#include "Transform.h"
class Bullet
{
public:
	void addHit();
	int getHits();
	int getMaxHits();
	void setHits(int);
	glm::vec3 getVelocity();
	void setVelocity(glm::vec3);
	void addPosition(Transform*);
	float bulletSpeed = 0.009;//0.009
	bool isActive = false;
	bool isAlreadyInObsticle = false;
protected:
	int maxHits = 3;//3
	int currentHits = 0;
	glm::vec3 velocity = glm::vec3(0);

	
};

