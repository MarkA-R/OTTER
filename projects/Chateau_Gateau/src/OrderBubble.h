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
	MaterialCreator* orderTiles[4];
	MaterialCreator* plusTile;
	MaterialCreator* bubbleTile;
	std::unique_ptr<Entity> bubble;
	std::unique_ptr<Entity> plusL, plusR, plusM;
	std::unique_ptr<Entity> pastryTile, fillingTile, toppingTile, drinkTile;
	Transform bubbleTransform;
	std::vector<Entity*> toRender = std::vector<Entity*>();
	//if nullptr dont render
public:
	OrderBubble(OvenTimer*);
	void setFill(float);
	void addFill(float);
	bool isOrderExpired();
	bool isOrderCompleted(Pastry&);
	void setTransform(Transform& t);
	void setTiles(MaterialCreator*, MaterialCreator*, MaterialCreator*, MaterialCreator*);//2
	void setup(MaterialCreator*, MaterialCreator*);//3
	void setupTimer(OvenTimer*);
	
	void create(Order& o);
	Entity* getBubble();
	std::vector<Entity*> returnRenderingEntities();
	void clearRenderingEntities();
	Transform* getTransform();
	Order* getOrder();
};

