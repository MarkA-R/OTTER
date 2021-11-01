#include "Bullet.h"

void Bullet::addHit()
{
	currentHits++;
}

int Bullet::getHits()
{
	return currentHits;
}

int Bullet::getMaxHits()
{
	return maxHits;
}

void Bullet::setHits(int i) {
	currentHits = i;
}

glm::vec3 Bullet::getVelocity()
{
	return velocity;
}

void Bullet::setVelocity(glm::vec3 x)
{
	velocity = x;
}

void Bullet::addPosition(Transform* t)
{
	t->addPosition(velocity);
}
