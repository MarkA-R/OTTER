#include "OrderBubble.h"

OrderBubble::OrderBubble(OvenTimer* ovenTimer)
{
    timer = ovenTimer;
    bubbleTransform = timer->getTransform();

}

void OrderBubble::setFill(float x)
{
    timer->setFill(x);
}

void OrderBubble::addFill(float x)
{
    float realFill = 0;
    float totalSeconds = (order.startTime + order.workTime) - order.startTime;
    float secondsPassed = bakeryUtils::getTime() - order.startTime;
  
    realFill = secondsPassed / totalSeconds;
    //std::cout << realFill << std::endl;
    if (realFill > 1.f) {
        realFill = 1.f;
    }
    timer->setFill(realFill);
    timer->updateArrow();
}

bool OrderBubble::isOrderExpired()
{
    //std::cout << order.maxEndTime << std::endl;
    return !order.isOnTime();
}

bool OrderBubble::isOrderCompleted(Pastry& p)
{
    return order.validateOrder(p);
}

void OrderBubble::setTransform(Transform& t)
{
    bubbleTransform = t;
}

void OrderBubble::setTiles(MaterialCreator* pastry, MaterialCreator* fill, MaterialCreator* top, MaterialCreator* drink)
{
    orderTiles[0] = pastry;
    orderTiles[1] = fill;
    orderTiles[2] = top;
    orderTiles[3] = drink;
}

void OrderBubble::setup(MaterialCreator* bubble, MaterialCreator* plus)
{
    bubbleTile = bubble;
    plusTile = plus;
}

void OrderBubble::setupTimer(OvenTimer* t)
{
    //timer = OvenTimer(t, a, c, bubbleTransform);
   timer = t;

  
}

void OrderBubble::create(Order& o, float scaleAll)
{
    order = o;
    float offset = 0.05 * bubbleTransform.m_scale.x;
    float totalEnties = 1;//for pastry
    bool hasTopping = false;
    bool hasFilling = false;
    bool hasDrink = false;
    float scaleX = 1.3;
    float scaleY = 0.75;
   // float scaleAll = 0.95;
    float place = 1;
    float yAdder = 0.15;
    if (order.filling != bakeryUtils::fillType::NONE) {
        totalEnties += 1;
        hasFilling = true;
    }
    if (order.topping != bakeryUtils::toppingType::NONE) {
        totalEnties += 1;
        hasTopping = true;
    }
    if (order.drink != bakeryUtils::drinkType::NONE) {
       // totalEnties += 1;
        hasDrink = true;
    }
    float width = ((scaleX-0.05)*(scaleAll-0.05)) * 2;
    //std::cout << "W " << width << std::endl;
    
    float sectionWidth = (width / totalEnties);
    //std::cout << "SECW " << sectionWidth << std::endl;
    timer->setFill(0);
    timer->updateArrow();
    toRender.push_back(timer->getArrow());
    toRender.push_back(timer->getCircle());
    toRender.push_back(timer->getTile());
    timer->getArrow()->transform.m_scale *= (scaleAll + 0.05);
    timer->getCircle()->transform.m_scale *= (scaleAll + 0.05);
    timer->getTile()->transform.m_scale *= (scaleAll + 0.05);
   
    glm::vec3 timerPos = bubbleTransform.m_pos;
    timerPos.y -= (0.29 * scaleX * (scaleAll + 0.05));
    timerPos.x -= (0.70 * scaleX * (scaleAll + 0.05));
   // std::cout << timerPos.y << std::endl;
    timer->setPosition(timerPos);
    timer->setTransform(bubbleTransform);
   
    

    bubble = Entity::Allocate();
    bubble.get()->Add<CMeshRenderer>(*bubble, *bubbleTile->getMesh(), *bubbleTile->getMaterial());
    bubble.get()->transform.m_scale = glm::vec3(scaleX, scaleX, scaleY) * scaleAll;
    bubble.get()->transform.m_rotation =
        glm::angleAxis(glm::radians(-180.f), glm::vec3(0.0f, 0.0f, 1.0f))*
        glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::angleAxis(glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f));

   
    bubble.get()->transform.m_pos = bubbleTransform.m_pos;
    bubble.get()->transform.SetParent(&bubbleTransform);
    toRender.push_back(bubble.get());
    glm::vec3 bubblePos = bubbleTransform.m_pos;


  //  std::cout << "SECWPLACE " << sectionWidth * place << std::endl;
   // std::cout << "SECW/2 " << sectionWidth / 2 << std::endl;
   // std::cout << "BPOS " << bubblePos.x << std::endl;
    //std::cout << "FINAL " << (bubblePos.x - (width / 2)) + ((sectionWidth / 2) * place) << std::endl;

    if (hasDrink) {

        drinkTile = Entity::Allocate();
        drinkTile.get()->Add<CMeshRenderer>(*drinkTile, *orderTiles[3]->getMesh(), *orderTiles[3]->getMaterial());
        drinkTile.get()->transform.SetParent(&bubbleTransform);
        drinkTile.get()->transform.m_rotation =
            glm::angleAxis(glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));// *
        drinkTile.get()->transform.m_scale = (glm::vec3(0.7, 0.7, 0.7) * 0.3f) * scaleAll;


        drinkTile.get()->transform.m_pos = glm::vec3((bubblePos.x - (width / 2)) + (((width/2) / 2) * (1.5)), bubblePos.y - (yAdder * 1.2), bubblePos.z - 0.01);
        toRender.push_back(drinkTile.get());
        //place++;

        plusR = Entity::Allocate();
        plusR.get()->Add<CMeshRenderer>(*plusR, *plusTile->getMesh(), *plusTile->getMaterial());
        plusR.get()->transform.SetParent(&bubbleTransform);
        plusR.get()->transform.m_rotation =
            glm::angleAxis(glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));// *
        plusR.get()->transform.m_scale = (glm::vec3(0.8, 0.8, 0.8) * 0.1f)* scaleAll;


        plusR.get()->transform.m_pos = glm::vec3((bubblePos.x - (width / 2)) + (((width / 2) / 2) * 2.5), bubblePos.y - (yAdder * 1.2), bubblePos.z - 0.01);
        toRender.push_back(plusR.get());
        //place++;


    }


    if (hasTopping) {

        toppingTile = Entity::Allocate();
        toppingTile.get()->Add<CMeshRenderer>(*toppingTile, *orderTiles[2]->getMesh(), *orderTiles[2]->getMaterial());
        toppingTile.get()->transform.SetParent(&bubbleTransform);
        toppingTile.get()->transform.m_rotation =
            glm::angleAxis(glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));// *
        toppingTile.get()->transform.m_scale = (glm::vec3(0.8, 0.8, 0.8) * 0.3f)* scaleAll;


        toppingTile.get()->transform.m_pos = glm::vec3((bubblePos.x - (width / 2)) + ((sectionWidth / 2) * place), bubblePos.y + yAdder, bubblePos.z - 0.01);
        toRender.push_back(toppingTile.get());
        place++;

        plusM = Entity::Allocate();
        plusM.get()->Add<CMeshRenderer>(*plusM, *plusTile->getMesh(), *plusTile->getMaterial());
        plusM.get()->transform.SetParent(&bubbleTransform);
        plusM.get()->transform.m_rotation =
            glm::angleAxis(glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));// *
        plusM.get()->transform.m_scale = (glm::vec3(0.8, 0.8, 0.8) * 0.1f)* scaleAll;


        plusM.get()->transform.m_pos = glm::vec3((bubblePos.x - (width / 2)) + ((sectionWidth / 2) * place), bubblePos.y + yAdder, bubblePos.z - 0.01);
        toRender.push_back(plusM.get());
        place++;

       
    }

    if (hasFilling) {

        fillingTile = Entity::Allocate();
        fillingTile.get()->Add<CMeshRenderer>(*fillingTile, *orderTiles[1]->getMesh(), *orderTiles[1]->getMaterial());
        fillingTile.get()->transform.SetParent(&bubbleTransform);
        fillingTile.get()->transform.m_rotation =
            glm::angleAxis(glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));// *
        fillingTile.get()->transform.m_scale = (glm::vec3(0.8, 0.8, 0.8) * 0.3f)* scaleAll;


        fillingTile.get()->transform.m_pos = glm::vec3((bubblePos.x - (width / 2)) + ((sectionWidth / 2) * place), bubblePos.y + yAdder, bubblePos.z - 0.01);
        toRender.push_back(fillingTile.get());
        place++;

        plusL = Entity::Allocate();
        plusL.get()->Add<CMeshRenderer>(*plusL, *plusTile->getMesh(), *plusTile->getMaterial());
        plusL.get()->transform.SetParent(&bubbleTransform);
        plusL.get()->transform.m_rotation =
            glm::angleAxis(glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));// *
        plusL.get()->transform.m_scale = (glm::vec3(0.8, 0.8, 0.8) * 0.1f)* scaleAll;


        plusL.get()->transform.m_pos = glm::vec3((bubblePos.x - (width / 2)) + ((sectionWidth / 2) * place), bubblePos.y + yAdder, bubblePos.z - 0.01);
        toRender.push_back(plusL.get());
        place++;

        
    }

    pastryTile = Entity::Allocate();
    pastryTile.get()->Add<CMeshRenderer>(*pastryTile, *orderTiles[0]->getMesh(), *orderTiles[0]->getMaterial());
    pastryTile.get()->transform.SetParent(&bubbleTransform);
    pastryTile.get()->transform.m_rotation =
        glm::angleAxis(glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
        
       // * glm::angleAxis(glm::radians(45.f), glm::vec3(0.0f, 0.0f, 1.0f));// *
    pastryTile.get()->transform.m_scale = (glm::vec3(0.8, 0.8, 0.8) * 0.3f)* scaleAll;
   
    pastryTile.get()->transform.m_pos = glm::vec3((bubblePos.x - (width/2)) + ((sectionWidth/2)*place), bubblePos.y + yAdder, bubblePos.z - 0.01);
    toRender.push_back(pastryTile.get());
  
   

    

    

}

Entity* OrderBubble::getBubble()
{
    return bubble.get();
}

std::vector<Entity*> OrderBubble::returnRenderingEntities()
{
    return toRender;
}

void OrderBubble::clearRenderingEntities()
{
    toRender.clear();
    fillingTile = nullptr;
    toppingTile = nullptr;
    pastryTile = nullptr;
    order.setOver(false);
}

Transform* OrderBubble::getTransform()
{
    return &bubbleTransform;
}

Order* OrderBubble::getOrder()
{
    // TODO: insert return statement here
    return &order;
}

float OrderBubble::getFill()
{
    return timer->getFill();
}

OvenTimer& OrderBubble::getTimer()
{
    return *timer;
}


