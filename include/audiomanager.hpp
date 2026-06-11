#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>

class AudioManager {
private:
    std::unordered_map<std::string, Sound> sounds;

public:
    AudioManager();
    ~AudioManager();

    void LoadSoundEffect(const std::string& key, const std::string& filename);
    void PlaySoundEffect(const std::string& key);
};