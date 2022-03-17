#pragma once
#include "ToneFire.h"
class Music
{
public:
	Music(ToneFire::StudioSound tfFile);
protected:
	bool isPlaying = false;
	ToneFire::StudioSound* song;
	float volume = 0.f;
};

