/*#pragma once

//https://codyclaborn.me/tutorials/making-a-basic-fmod-audio-engine-in-c/
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include "GLM/glm.hpp"
struct Implementation {
    Implementation();
    ~Implementation();

    void Update();

    FMOD::Studio::System* studioSystem;
    FMOD::System* system;

    int nextChannel;

    typedef std::map<std::string, FMOD::Sound*> SoundMap;
    typedef std::map<int, FMOD::Channel*> ChannelMap;
    typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
    typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;

    BankMap bankMap;
    EventMap eventMap;
    SoundMap soundMap;
    ChannelMap channelMap;

    
};
Implementation* implementation = nullptr;
class Audio {
protected:
  
public:
    

    static void Init();
    static void Update();
    static void Quit();
    static int outputErrors(FMOD_RESULT result);

    void loadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    void loadEvent(const std::string& strEventName);
    void loadSound(const std::string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
    void unloadSound(const std::string& strSoundName);
    void setListenerAndOrientation(const glm::vec3& vPos = glm::vec3( 0, 0, 0 ), float fVolumedB = 0.0f);
    int playSound(const std::string& strSoundName, const glm::vec3& vPos = glm::vec3( 0, 0, 0 ), float fVolumedB = 0.0f);
    void playEvent(const std::string& strEventName);
    void stopChannel(int nChannelId);
    void stopEvent(const std::string& strEventName, bool bImmediate = false);
    void getEventParameter(const std::string& strEventName, const std::string& strEventParameter, float* parameter);
    void setEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue);
    void stopAllChannels();
    void setChannel3dPosition(int nChannelId, const glm::vec3& vPosition);
    void setChannelVolume(int nChannelId, float fVolumedB);
    bool isPlaying(int nChannelId) const;
    bool isEventPlaying(const std::string& strEventName) const;
    float dbToVolume(float db);
    float volumeTodb(float volume);
    FMOD_VECTOR vectorToFmod(const glm::vec3& vPosition);
};
*/
