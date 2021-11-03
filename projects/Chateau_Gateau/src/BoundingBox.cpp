#include "BoundingBox.h"

BoundingBox::BoundingBox(float a, Entity& o)
{
    owner = &o;
    reachX = a;
    reachY = a;
    reachZ = a;

}

BoundingBox::BoundingBox(glm::vec3 a, Entity& o)
{
    owner = &o;
    reachX = a.x;
    reachY = a.y;
    reachZ = a.z;
}

BoundingBox::BoundingBox(float x, float y, float z, Entity& o)
{
    owner = &o;
    reachX = x;
    reachY = y;
    reachZ = z;
}

glm::vec3 BoundingBox::getOrigin()
{
    return owner->transform.m_pos;
}

bool BoundingBox::isColliding(glm::vec3 point)//from https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
{
    glm::vec3 origin = getOrigin();

    return (point.x >= origin.x - reachX && point.x <= origin.x + reachX) &&
        (point.y >= origin.y - reachY && point.y <= origin.y + reachY) &&
        (point.z >= origin.z - reachZ && point.z <= origin.z + reachZ);
}
