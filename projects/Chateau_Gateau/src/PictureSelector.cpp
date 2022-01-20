#include "PictureSelector.h"

PictureSelector::PictureSelector(Entity* e)
{
	owner = e;
}

void PictureSelector::setPictures(std::vector<MaterialCreator*> pics)
{
	allMaterials = pics;
}

void PictureSelector::setIndex(int i)
{
	currentOption = i;
	if (currentOption >= allMaterials.size()) {
		currentOption = allMaterials.size();
	}
	else if (currentOption < 0) {
		currentOption = 0;
	}
}

void PictureSelector::addToIndex(int i )
{
	int prevSelected = currentOption;
	int adder = 0;
	if (i < 0) {
		adder = allMaterials.size() - 1;
	}

	if (i > 0) {
		adder = i;
	}
	currentOption = (prevSelected + adder) % allMaterials.size();
}

int PictureSelector::getIndex()
{
	return currentOption;
}

void PictureSelector::updatePicture()
{
	if (owner->Has<CMeshRenderer>()) {
		owner->Get<CMeshRenderer>().SetMaterial(*allMaterials[currentOption]->getMaterial());
	}
}
