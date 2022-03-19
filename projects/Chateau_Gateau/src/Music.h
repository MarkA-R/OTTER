#pragma once
#include "ToneFire.h"
class Music
{
public:
	Music(ToneFire::StudioSound& tfFile, std::string n, float sec, bool useSoundVolume = false);
	void update(float dt);//for fades
	void fadeIn(float time);
	void fadeOut(float time);
	void setLoop(bool toLoop);
protected:
	bool isPlaying = false;
	bool shouldLoop = false;
	ToneFire::StudioSound* song;
	float volumeMulti = 1;
	float volChangeTime = 0.f;
	float seconds = 0.f;
	float volumeT = 0.f;
	std::string name;
	float secondsCounter = 0.f;
	bool isUsingSound = false;
};

