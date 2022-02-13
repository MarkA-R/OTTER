#include "Audio.h"
#include "fmod_errors.h"
void Audio::Init()
{
	FMOD::System_Create(&system);
	system->init(16, FMOD_INIT_NORMAL, nullptr);
}

void Audio::Update()
{
	system->update();
}

void Audio::Quit()
{
	system->close();
	system->release();
}

void Audio::loadSound(const std::string& soundName, const std::string& fileName, bool b3d, bool looping, bool stream)
{
	auto foundSound = sounds.find(soundName);
	if (foundSound != sounds.end()) {
		return;
	}
	FMOD_MODE mode = FMOD_DEFAULT;
	mode |= (b3d) ? FMOD_3D : FMOD_2D;
	mode |= (looping) ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	mode |= (stream) ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* loadedSound;
	outputErrors(system->createSound(fileName.c_str(), FMOD_3D, nullptr, &loadedSound));
	if (loadedSound != nullptr) {
		sounds[soundName] = loadedSound;
	}
}

void Audio::unloadSound(const std::string& soundName)
{
	auto foundSound = sounds.find(soundName);
	if (foundSound != sounds.end()) {
		//return;
		outputErrors(foundSound->second->release());
		sounds.erase(foundSound);
	}
}

void Audio::playSound(const std::string& soundName)
{
	outputErrors(system->playSound(sounds[soundName], nullptr, false, nullptr));
}

int Audio::outputErrors(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		std::cout << "FMOD ERROR - " << FMOD_ErrorString(result) << std::endl;

#ifdef _DEBUG
		__debugbreak();
#endif
		return 1;
	}
	return 0;
}
