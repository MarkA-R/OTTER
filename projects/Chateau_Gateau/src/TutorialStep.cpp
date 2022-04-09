#include "TutorialStep.h"

TutorialStep::TutorialStep(MaterialCreator* material, bool affectedBySpace, int si)
{
	nextMaterial = material;
	isAffectedBySpace = affectedBySpace;
	canContinue = false;
	soundIndex = si;
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

int TutorialStep::getSoundIndex()
{
	return soundIndex;
}

MaterialCreator* TutorialStep::getMaterialCreator()
{
	return nextMaterial;
}
