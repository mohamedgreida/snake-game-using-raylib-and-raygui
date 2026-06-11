#include "AudioManager.hpp"

AudioManager::AudioManager() {
    InitAudioDevice();
    LoadSoundEffect("Move","Move 2.wav");
    LoadSoundEffect("Eat", "Eat.wav");
}

AudioManager::~AudioManager() {
    for (auto& pair : sounds) {
        UnloadSound(pair.second);
    }
    CloseAudioDevice();
}

void AudioManager::LoadSoundEffect(const std::string& key, const std::string& filename) {
    if (sounds.find(key) == sounds.end()) {
        sounds[key] = LoadSound(TextFormat("%s%s", RESOURCES_PATH, filename.c_str()));
    }
}

void AudioManager::PlaySoundEffect(const std::string& key) {
    if (sounds.find(key) != sounds.end()) {
        if(key == "Move"){
            SetMasterVolume(1.4);
        }else{
            SetMasterVolume(1);
        }
        PlaySound(sounds[key]);
    }
}