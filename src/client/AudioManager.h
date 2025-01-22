#pragma once
#include "ResourceManager.h"
#include <SFML/Audio/SoundBuffer.hpp>

class AudioManager : public ResourceManager<AudioManager, sf::SoundBuffer> {
public:
    AudioManager() : ResourceManager("audio.cfg") {};

    sf::SoundBuffer* Load(const std::string& l_path) {
        sf::SoundBuffer* sound = new sf::SoundBuffer();
        if (!sound->loadFromFile(Utils::GetWorkingDirectory() + l_path)) {
            std::cout << "Failed to load sound buffer from: " << l_path << std::endl;
            delete sound;
            sound = nullptr;
        }
        return sound;
    };
};