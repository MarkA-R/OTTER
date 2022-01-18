#pragma once
#include "NOU/Entity.h"
#include "NOU/CMeshRenderer.h"
#include "MotionTable.h"
#include "MaterialCreator.h"
using namespace nou;
class PictureSelector
{
	//make an entity list for the picture selectors and put them at the board
	//turn them off until you select options
	//then when you select to change tray hotkeys the tray comes in, and the entities' transforms are set to the tray slots
	//they will render in order of the tray, so to start it will be with all but the first slots on, then the second slots will come on along with teh first, and so on
	//then on the fourth one, you exit when you press another key

	//for the other ones, you use space to select and the arrow keys to go up or down
	//and press space on the back button (thats on the board) to go back to the main menu

private:
	std::vector<MaterialCreator*> allMaterials = std::vector<MaterialCreator*>();
	int currentOption = 0;
	Entity* owner;
public:
	PictureSelector(Entity*);
	void setPictures(std::vector<MaterialCreator*>);
	void setIndex(int);
	void addToIndex(int);
	int getIndex();
	void updatePicture();

};

