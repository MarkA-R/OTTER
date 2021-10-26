#pragma once
#include "globalHeaders.h"
class Entity
{
public:
	Entity(entt::entity);
	~Entity();
	int getID();
	entt::entity getEntity();
protected:
	int id;
	entt::entity* ent;


};

