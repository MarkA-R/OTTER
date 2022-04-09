#pragma once
#include "NOU/Entity.h"
#include "MaterialCreator.h"
#include "NOU/CMeshRenderer.h"
#include <iostream>
using namespace nou;
class TutorialStep
{
protected:
	MaterialCreator* nextMaterial;
	bool canContinue = false;
	bool isAffectedBySpace = false;
	int soundIndex = -1;

public:
	TutorialStep(MaterialCreator* material,bool affectedBySpace, int si = -1);
	void setContinueState(bool);
	bool getContinueState();
	bool spaceContinues();
	int getSoundIndex();
	MaterialCreator* getMaterialCreator();
};

