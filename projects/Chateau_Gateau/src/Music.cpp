#include "Music.h"
#include "bakeryUtils.h"

Music::Music(ToneFire::StudioSound& tfFile, std::string n, float sec, bool isSound)
{
	seconds = sec;
	song = &tfFile;
	name = n;
	isUsingSound = isSound;
}

void Music::update(float dt)
{
	if (volChangeTime != 0.f) {
		volumeT += ((dt / volChangeTime) * volumeMulti);
	}
	
	if (isPlaying) {
		secondsCounter += dt;
	}
	if (volumeT > 1.f) {
		volumeT = 1.f;
	}
	if (volumeT < 0.f) {
		volumeT = 0.f;
	}
	float usingVol = bakeryUtils::getMusicVolume();
	if (isUsingSound) {
		usingVol = bakeryUtils::getSoundVolume();
	}
	if (usingVol < 0.05f) {
		volumeT = 0;
	}
	if (volChangeTime != 0.f) {
		float newVol = usingVol * (volumeT);
		std::string soundType = "parameter:/musicVolume";
		if (isUsingSound) {
			 soundType = "parameter:/soundVolume";
		}
		song->SetEventParameter("event:/" + name, soundType, newVol);
		if (usingVol < 0.01f && isPlaying) {
			song->StopEvent("event:/" + name);
			isPlaying = false;
			secondsCounter = seconds + 1;
		}
		else if(usingVol > 0.01f && !isPlaying && (secondsCounter <= seconds || shouldLoop))
		{
			song->PlayEvent("event:/" + name);
			isPlaying = true;
			secondsCounter = 0;
			if (shouldLog) {
				std::cout << "USING VOL" << std::endl;
			}
			

		}
		//std::cout << newVol << std::endl;
	}
	bool looping = false;
	
		if (secondsCounter >= seconds) {
			
			volumeT = 0.f;
			isPlaying = false;
			if (shouldLoop) {
				secondsCounter = 0;
				looping = true;
				
			}
			
		}
		
	
	if (volumeT == 0.f) {
		song->StopEvent("event:/" + name);
		isPlaying = false;
		
	}
	else
	{
		isPlaying = true;
		if (shouldLog) {
			std::cout << "VOLT" << std::endl;
		}
		

	}

	if (looping) {
		
		volumeMulti = 1;

		song->PlayEvent("event:/" + name);
		isPlaying = true;
		if (shouldLog) {
			std::cout << "LOOPING" << std::endl;
		}
		
	}
	
}

void Music::fadeIn(float time)
{
	
		volumeMulti = 1;
		volChangeTime = time;
		secondsCounter = 0;
		song->PlayEvent("event:/" + name);
	
	
}

void Music::fadeOut(float time)
{
	volumeMulti = -1;
	volChangeTime = time;
}

void Music::setLoop(bool toLoop)
{
	shouldLoop = toLoop;
}

bool Music::isCurrentlyPlaying()
{
	return isPlaying;
}

ToneFire::StudioSound& Music::getSound()
{
	return *song;
}

