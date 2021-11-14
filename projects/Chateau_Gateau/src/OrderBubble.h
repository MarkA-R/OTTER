#pragma once
#include "Pastry.h"
#include "Order.h"
#include "OvenTimer.h"
#include "NOU/Entity.h"
#include "bakeryUtils.h"
#include "NOU/GLTFLoader.h"
#include "MaterialCreator.h"

using namespace nou;
class OrderBubble
{
private:
	OvenTimer* timer;
	Order order;
	MaterialCreator* orderTiles[3];
	MaterialCreator* plusTile;
	MaterialCreator* bubbleTile;
	std::unique_ptr<Entity> bubble;
	std::unique_ptr<Entity> plusL, plusR;
	std::unique_ptr<Entity> pastryTile, fillingTile, toppingTile;
	Transform bubbleTransform;
	std::vector<Entity*> toRender = std::vector<Entity*>();
	//if nullptr dont render
public:
	OrderBubble(OvenTimer*);
	void setFill(float);
	void addFill(float);
	bool isOrderExpired();
	bool isOrderCompleted(Pastry&);
	void setTransform(Transform& t);//DEPRICIATED
	void setTiles(MaterialCreator*, MaterialCreator*, MaterialCreator*);//2
	void setup(MaterialCreator*, MaterialCreator*);//3
	void setupTimer(MaterialCreator& t, MaterialCreator& a, MaterialCreator& c);//DEPRICIATED
	void create(Order& o);
	Entity* getBubble();
	std::vector<Entity*> returnRenderingEntities();
	Transform* getTransform();
};

