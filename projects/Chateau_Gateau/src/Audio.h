#pragma once

//https://codyclaborn.me/tutorials/making-a-basic-fmod-audio-engine-in-c/
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include "GLM/glm.hpp"
#include <unordered_map>

class Audio
{
public:
	void Init();
	void Update();
	void Quit();
	void loadSound(const std::string& soundName, const std::string& fileName, bool b3d, bool looping, bool stream);
	void unloadSound(const std::string& soundName);
	void playSound(const std::string& soundName);
	int outputErrors(FMOD_RESULT result);
	FMOD::System* system;
	std::unordered_map<std::string, FMOD::Sound*> sounds;
};

