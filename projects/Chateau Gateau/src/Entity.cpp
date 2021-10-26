#include "Entity.h"
Entity::Entity(entt::entity e) {
	id = entt::registry().alive() + 1;
	ent = &e;

}

Entity::~Entity() {
	//entt::registry().destroy(id - 1, id - 1);
	//entt::registry().
}
int Entity::getID() {
	return id;
}

entt::entity Entity::getEntity() {
	return *ent;
}

