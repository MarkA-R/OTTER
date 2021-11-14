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
    timer->addFill(x);
}

bool OrderBubble::isOrderExpired()
{
    return order.isOnTime();
}

bool OrderBubble::isOrderCompleted(Pastry& p)
{
    return order.validateOrder(p);
}

void OrderBubble::setTransform(Transform& t)
{
    bubbleTransform = t;
}

void OrderBubble::setTiles(MaterialCreator* pastry, MaterialCreator* fill, MaterialCreator* top)
{
    orderTiles[0] = pastry;
    orderTiles[1] = fill;
    orderTiles[2] = top;
}

void OrderBubble::setup(MaterialCreator* bubble, MaterialCreator* plus)
{
    bubbleTile = bubble;
    plusTile = plus;
}

void OrderBubble::setupTimer(MaterialCreator& t, MaterialCreator& a, MaterialCreator& c)
{
    //timer = OvenTimer(t, a, c, bubbleTransform);
    
}

void OrderBubble::create(Order& o)
{
    order = o;
    float offset = 0.05 * bubbleTransform.m_scale.x;
    float totalEnties = 1;//for pastry
    bool hasTopping = false;
    bool hasFilling = false;
    float scaleX = 1.3;
    float scaleAll = 0.9;
    float place = 1;
    float yAdder = 0.1;
    if (order.filling != bakeryUtils::fillType::NONE) {
        totalEnties += 1;
        hasFilling = true;
    }
    if (order.topping != bakeryUtils::toppingType::NONE) {
        totalEnties += 1;
        hasTopping = true;
    }
    float width = (scaleX*(scaleAll-0.05)) * 2;
    std::cout << "W " << width << std::endl;
    
    float sectionWidth = (width / totalEnties);
    std::cout << "SECW " << sectionWidth << std::endl;

    toRender.push_back(timer->getArrow());
    toRender.push_back(timer->getCircle());
    toRender.push_back(timer->getTile());
    glm::vec3 timerPos = bubbleTransform.m_pos;
    timerPos.y -= 0.3;
    timer->setPosition(timerPos);
    timer->setTransform(bubbleTransform);
   
    

    bubble = Entity::Allocate();
    bubble.get()->Add<CMeshRenderer>(*bubble, *bubbleTile->getMesh(), *bubbleTile->getMaterial());
    bubble.get()->transform.m_rotation =
        glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::angleAxis(glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f));// *
        //;
    bubble.get()->transform.m_scale = glm::vec3(scaleX, 0.8, 0.8) * scaleAll;
    bubble.get()->transform.m_pos = bubbleTransform.m_pos;
    bubble.get()->transform.SetParent(&bubbleTransform);
    toRender.push_back(bubble.get());
    glm::vec3 bubblePos = bubbleTransform.m_pos;


    std::cout << "SECWPLACE " << sectionWidth * place << std::endl;
    std::cout << "SECW/2 " << sectionWidth / 2 << std::endl;
    std::cout << "BPOS " << bubblePos.x << std::endl;
    std::cout << "FINAL " << (bubblePos.x - (width / 2)) + ((sectionWidth / 2) * place) << std::endl;

    pastryTile = Entity::Allocate();
    pastryTile.get()->Add<CMeshRenderer>(*pastryTile, *orderTiles[0]->getMesh(), *orderTiles[0]->getMaterial());
    pastryTile.get()->transform.SetParent(&bubbleTransform);
    pastryTile.get()->transform.m_rotation =
        glm::angleAxis(glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
        
       // * glm::angleAxis(glm::radians(45.f), glm::vec3(0.0f, 0.0f, 1.0f));// *
    pastryTile.get()->transform.m_scale = glm::vec3(0.8, 0.8, 0.8) * 0.3f;
   
    pastryTile.get()->transform.m_pos = glm::vec3((bubblePos.x - (width/2)) + ((sectionWidth/2)*place), bubblePos.y + yAdder, bubblePos.z - 0.01);
    toRender.push_back(pastryTile.get());
    place++;
    if (hasFilling) {

        plusL = Entity::Allocate();
        plusL.get()->Add<CMeshRenderer>(*plusL, *plusTile->getMesh(), *plusTile->getMaterial());
        plusL.get()->transform.SetParent(&bubbleTransform);
        plusL.get()->transform.m_rotation =
            glm::angleAxis(glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));// *
        plusL.get()->transform.m_scale = glm::vec3(0.8, 0.8, 0.8) * 0.1f;

       
        plusL.get()->transform.m_pos = glm::vec3((bubblePos.x - (width / 2)) + ((sectionWidth / 2) * place), bubblePos.y + yAdder, bubblePos.z - 0.01);
        toRender.push_back(plusL.get());
        place++;

        fillingTile = Entity::Allocate();
        fillingTile.get()->Add<CMeshRenderer>(*fillingTile, *orderTiles[1]->getMesh(), *orderTiles[1]->getMaterial());
        fillingTile.get()->transform.SetParent(&bubbleTransform);
        fillingTile.get()->transform.m_rotation =
            glm::angleAxis(glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));// *
        fillingTile.get()->transform.m_scale = glm::vec3(0.8, 0.8, 0.8) * 0.3f;


        fillingTile.get()->transform.m_pos = glm::vec3((bubblePos.x - (width / 2)) + ((sectionWidth / 2) * place), bubblePos.y + yAdder, bubblePos.z - 0.01);
        toRender.push_back(fillingTile.get());
        place++;
    }

    if (hasTopping) {

        plusR = Entity::Allocate();
        plusR.get()->Add<CMeshRenderer>(*plusR, *plusTile->getMesh(), *plusTile->getMaterial());
        plusR.get()->transform.SetParent(&bubbleTransform);
        plusR.get()->transform.m_rotation =
            glm::angleAxis(glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));// *
        plusR.get()->transform.m_scale = glm::vec3(0.8, 0.8, 0.8) * 0.1f;


        plusR.get()->transform.m_pos = glm::vec3((bubblePos.x - (width / 2)) + ((sectionWidth / 2) * place), bubblePos.y + yAdder, bubblePos.z - 0.01);
        toRender.push_back(plusR.get());
        place++;

        toppingTile = Entity::Allocate();
        toppingTile.get()->Add<CMeshRenderer>(*toppingTile, *orderTiles[2]->getMesh(), *orderTiles[2]->getMaterial());
        toppingTile.get()->transform.SetParent(&bubbleTransform);
        toppingTile.get()->transform.m_rotation =
            glm::angleAxis(glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));// *
        toppingTile.get()->transform.m_scale = glm::vec3(0.8, 0.8, 0.8) * 0.3f;


        toppingTile.get()->transform.m_pos = glm::vec3((bubblePos.x - (width / 2)) + ((sectionWidth / 2) * place), bubblePos.y + yAdder, bubblePos.z - 0.01);
        toRender.push_back(toppingTile.get());
        place++;
    }

    

}

Entity* OrderBubble::getBubble()
{
    return bubble.get();
}

std::vector<Entity*> OrderBubble::returnRenderingEntities()
{
    return toRender;
}

Transform* OrderBubble::getTransform()
{
    return &bubbleTransform;
}


