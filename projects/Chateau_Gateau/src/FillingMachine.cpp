#include "FillingMachine.h"
#include <iostream>

void FillingMachine::setFrames(std::vector<Mesh*> usingFrames)
{
	frames = usingFrames;

}

void FillingMachine::setTransform(Transform& t)
{
	fillingTransform = t;
}

void FillingMachine::setCurrentFrame(int x)
{
	nextFrame = x;
}

void FillingMachine::setNextFrame(int x)
{
	nextFrame = x;
}

Mesh* FillingMachine::getFrame(int x)
{
	return frames[x];
}

int FillingMachine::getCurrentFrame()
{
	return currentFrame;
}

int FillingMachine::getNextFrame()
{
	return nextFrame;
}

void FillingMachine::setT(float x)
{
	if (x < 0) {
		x = 0;
	}
	if (x > 1) {
		x = 1;
	}
	fillingT = x;
}

float FillingMachine::getT()
{
	return fillingT;
}

bakeryUtils::fillType FillingMachine::getFilling()
{
	return bakeryUtils::fillType((selectedFilling) + 1);//+1 for NONE
}

void FillingMachine::addFillNum(int x)
{
	int prevSelected = selectedFilling;
	int adder = 0;
	if (x < 0) {
		adder = 2;
	}

	if (x > 0) {
		adder = 1;
	}
	selectedFilling = (prevSelected + adder) % 3;
}

void FillingMachine::setFillingPlane(Entity* x)
{
	fillingPlane = x;
}

Entity* FillingMachine::getFillingPlane()
{
	return fillingPlane;
}

void FillingMachine::updatePlane()
{
	//fillingPlane->Remove<CMeshRenderer>();
	
	//std::cout << selectedFilling << " " << (selectedFilling - 1) % 3 << std::endl;
	fillingPlane->Get<CMeshRenderer>().SetMaterial(*planeDesigns[selectedFilling]->getMaterial());
	//fillingPlane->Add<CMeshRenderer>(*fillingPlane, *planeDesigns[selectedFilling]->getMesh(), *planeDesigns[selectedFilling]->getMaterial());

}

void FillingMachine::setup(MaterialCreator* custard, MaterialCreator* notella, MaterialCreator* stawberry)
{
	planeDesigns[0] = custard;
	planeDesigns[1] = notella;
	planeDesigns[2] = stawberry;
}

bool FillingMachine::isFillingFull()
{
	if (inFilling == nullptr) {
		return false;
	}
	return true;
}

Entity* FillingMachine::getFromFilling()
{
	return inFilling;
}

void FillingMachine::removeFromFilling()
{
	inFilling = nullptr;

}


void FillingMachine::putInFilling(Entity* e)
{
	inFilling = e;
	//e->transform.m_pos = fillingTransform.m_pos;
	//e->transform.SetParent(nullptr);
}

glm::vec3 FillingMachine::getFillingPosition()
{
	return fillingTransform.m_pos;
}
