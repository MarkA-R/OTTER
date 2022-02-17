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

public:
	TutorialStep(MaterialCreator* material,bool affectedBySpace = false);
	void setContinueState(bool);
	bool getContinueState();
	bool spaceContinues();
	MaterialCreator* getMaterialCreator();
};

