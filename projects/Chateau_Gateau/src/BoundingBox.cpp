#include "BoundingBox.h"
#include <iostream>
BoundingBox::BoundingBox(float a, Entity& o)
{
    owner = &o;
    reachX = a;
    reachY = a;
    reachZ = a;
    origin = o.transform.m_pos;
}

BoundingBox::BoundingBox(glm::vec3 a, Entity& o)
{
    owner = &o;
    reachX = a.x;
    reachY = a.y;
    reachZ = a.z;
    origin = o.transform.m_pos;
}

BoundingBox::BoundingBox(float x, float y, float z, Entity& o)
{
    owner = &o;
    reachX = x;
    reachY = y;
    reachZ = z;
    origin = o.transform.m_pos;
}

glm::vec3 BoundingBox::getOrigin()
{
    return origin;
}

void BoundingBox::setOrigin(glm::vec3 x)
{
    origin = x;
}

bool BoundingBox::isColliding(glm::vec3 point)//from https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
{
   
    //std::cout << ori.x << " " << ori.y << " " << ori.z << std::endl;
    return (point.x >= origin.x - reachX && point.x <= origin.x + reachX) &&
        (point.y >= origin.y - reachY && point.y <= origin.y + reachY) &&
        (point.z >= origin.z - reachZ && point.z <= origin.z + reachZ);
}
