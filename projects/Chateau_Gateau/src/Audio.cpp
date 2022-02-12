/*
#include "Audio.h"

Implementation::Implementation()
{
   
    studioSystem = NULL;
    Audio::outputErrors(FMOD::Studio::System::create(&studioSystem));
    Audio::outputErrors(studioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

    system = NULL;
    Audio::outputErrors(studioSystem->getCoreSystem(&system));
    
}
Implementation::~Implementation() {
    Audio::outputErrors(studioSystem->unloadAll());
    Audio::outputErrors(studioSystem->release());
}

void Implementation::Update() {
    std::vector<ChannelMap::iterator> pStoppedChannels;
    for (auto it = channelMap.begin(), itEnd = channelMap.end(); it != itEnd; ++it)
    {
        bool bIsPlaying = false;
        it->second->isPlaying(&bIsPlaying);
        if (!bIsPlaying)
        {
            pStoppedChannels.push_back(it);
        }
    }
    for (auto& it : pStoppedChannels)
    {
        channelMap.erase(it);
    }
   Audio::outputErrors(studioSystem->update());

}

int Audio::outputErrors(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        std::cout << "FMOD ERROR " << result << std::endl;
        return 1;
    }
    // cout << "FMOD all good" << endl;
    return 0;
}

void Audio::Init() {
  
    implementation = new Implementation;
}

void Audio::Update() {
  implementation->Update();
}

void Audio::loadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
{
    auto tFoundIt = implementation->soundMap.find(strSoundName);
    if (tFoundIt != implementation->soundMap.end())
        return;

    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= b3d ? FMOD_3D : FMOD_2D;
    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    FMOD::Sound* pSound = nullptr;
    Audio::outputErrors(implementation->system->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
    if (pSound) {
        implementation->soundMap[strSoundName] = pSound;
    }

}

void Audio::unloadSound(const std::string& strSoundName)
{
    auto tFoundIt = implementation->soundMap.find(strSoundName);
    if (tFoundIt == implementation->soundMap.end())
        return;

    Audio::outputErrors(tFoundIt->second->release());
    implementation->soundMap.erase(tFoundIt);
}

int Audio::playSound(const std::string& strSoundName, const glm::vec3& vPosition, float fVolumedB)
{
    int nChannelId = implementation->nextChannel++;
    auto tFoundIt = implementation->soundMap.find(strSoundName);
    if (tFoundIt == implementation->soundMap.end())
    {
        loadSound(strSoundName);
        tFoundIt = implementation->soundMap.find(strSoundName);
        if (tFoundIt == implementation->soundMap.end())
        {
            return nChannelId;
        }
    }
    FMOD::Channel* pChannel = nullptr;
    Audio::outputErrors(implementation->system->playSound(tFoundIt->second, nullptr, true, &pChannel));
    if (pChannel)
    {
        FMOD_MODE currMode;
        tFoundIt->second->getMode(&currMode);
        if (currMode & FMOD_3D) {
            FMOD_VECTOR position = vectorToFmod(vPosition);
            Audio::outputErrors(pChannel->set3DAttributes(&position, nullptr));
        }
        Audio::outputErrors(pChannel->setVolume(dbToVolume(fVolumedB)));
        Audio::outputErrors(pChannel->setPaused(false));
        implementation->channelMap[nChannelId] = pChannel;
    }
    return nChannelId;
}

void Audio::setChannel3dPosition(int nChannelId, const glm::vec3& vPosition)
{
    auto tFoundIt = implementation->channelMap.find(nChannelId);
    if (tFoundIt == implementation->channelMap.end())
        return;

    FMOD_VECTOR position = vectorToFmod(vPosition);
    Audio::outputErrors(tFoundIt->second->set3DAttributes(&position, NULL));
}

void Audio::setChannelVolume(int nChannelId, float fVolumedB)
{
    auto tFoundIt = implementation->channelMap.find(nChannelId);
    if (tFoundIt == implementation->channelMap.end())
        return;

    Audio::outputErrors(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
}

void Audio::loadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
    auto tFoundIt = implementation->bankMap.find(strBankName);
    if (tFoundIt != implementation->bankMap.end())
        return;
    FMOD::Studio::Bank* pBank;
    Audio::outputErrors(implementation->studioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
    if (pBank) {
        implementation->bankMap[strBankName] = pBank;
    }
}
*/