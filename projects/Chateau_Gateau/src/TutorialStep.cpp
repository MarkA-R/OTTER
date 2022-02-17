#include "TutorialStep.h"

TutorialStep::TutorialStep(MaterialCreator* material, bool affectedBySpace)
{
	nextMaterial = material;
	isAffectedBySpace = affectedBySpace;
	canContinue = false;
}

void TutorialStep::setContinueState(bool b)
{
	canContinue = b;
}

bool TutorialStep::getContinueState()
{
	return canContinue;
}

bool TutorialStep::spaceContinues()
{
	return isAffectedBySpace;
}

MaterialCreator* TutorialStep::getMaterialCreator()
{
	return nextMaterial;
}
