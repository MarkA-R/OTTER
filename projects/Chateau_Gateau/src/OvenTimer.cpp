#include "OvenTimer.h"
#include <iostream>
OvenTimer::OvenTimer(MaterialCreator& t, MaterialCreator& a, MaterialCreator& c, Transform& pos, float multiplier, glm::quat arrowRotation
	)
{
	fill = 0;
	float transformScale = multiplier;
	tile = Entity::Allocate();
	tile.get()->Add<CMeshRenderer>(*tile, *t.getMesh(), *t.getMaterial());
	position = pos;
	tile.get()->transform.SetParent(&position);
	tile.get()->transform.m_pos = position.m_pos;
	tile.get()->transform.m_rotation =
		glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));// *
		//;
	tile.get()->transform.m_scale = glm::vec3(0.8, 0.8, 0.8) * transformScale;
	tileScale = glm::vec3(0.8, 0.8, 0.8) * transformScale;
	
	
	
	circle = Entity::Allocate();
	arrow = Entity::Allocate();
	circle.get()->transform.SetParent(&position);
	circle.get()->Add<CMeshRenderer>(*circle, *c.getMesh(), *c.getMaterial());
	
	circle.get()->transform.m_pos = position.m_pos;
	circle.get()->transform.m_scale = glm::vec3(1.0, 1.0, 1.0) * transformScale;
	circleScale = glm::vec3(1.0, 1.0, 1.0) * transformScale;
	circle.get()->transform.m_rotation =  
		glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 0.0f, 1.0f));

	


	arrow.get()->Add<CMeshRenderer>(*arrow, *a.getMesh(), *a.getMaterial());	
	arrow.get()->transform.SetParent(&position);
	arrow.get()->transform.m_scale = glm::vec3(0.1,0.1,0.1) * transformScale;
	arrowScale = glm::vec3(0.1, 0.5, 0.1) * transformScale;
	arrow.get()->transform.m_pos = position.m_pos;
	arrow.get()->transform.m_rotation = arrowRotation;
	arrowRot = arrowRotation;
	

}

void OvenTimer::addFill(float x)
{
	fill -= x;
}

float OvenTimer::getFill()
{
	return fill;
}

void OvenTimer::setFill(float x)
{
	fill = x;
}

void OvenTimer::setMaterial(MaterialCreator& t)
{
	tile.get()->Remove<CMeshRenderer>();
	tile.get()->Add<CMeshRenderer>(*tile, *t.getMesh(), *t.getMaterial());
}

Entity* OvenTimer::getTile()
{
	return tile.get();
}

Entity* OvenTimer::getArrow()
{
	return arrow.get();
}

Entity* OvenTimer::getCircle()
{
	return circle.get();
}

void OvenTimer::updateArrow(glm::vec3 axis)
{
	float n = fill;
	float whole;

	fill = std::modf(n, &whole);
	//std::cout << fill << std::endl;
	float angle = 360 * fill;
	arrow.get()->transform.m_rotation = arrowRot * glm::angleAxis(glm::radians(angle), axis);
}

Transform& OvenTimer::getTransform()
{
	// TODO: insert return statement here
	return position;
}

void OvenTimer::setTransform(Transform& x)
{
	position = x;
	tile.get()->transform.SetParent(&position);
	circle.get()->transform.SetParent(&position);
	arrow.get()->transform.SetParent(&position);
}

void OvenTimer::setPosition(glm::vec3 x)
{
	tile.get()->transform.m_pos = x;
	circle.get()->transform.m_pos = x;
	arrow.get()->transform.m_pos = x;
}

void OvenTimer::dontShow(int i)
{
	if (i == 0) {
		tile->transform.m_scale = glm::vec3(0);
	}
	if (i == 1) {
		arrow->transform.m_scale = glm::vec3(0);
	}
	if (i == 2) {
		circle->transform.m_scale = glm::vec3(0);
	}
}

glm::vec3 OvenTimer::getTileScale()
{
	return tileScale;
}

glm::vec3 OvenTimer::getArrowScale()
{
	return arrowScale;
}

glm::vec3 OvenTimer::getCircleScale()
{
	return circleScale;
}




