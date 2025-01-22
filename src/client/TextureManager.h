#pragma once
#include "ResourceManager.h"
#include <SFML/Graphics/Texture.hpp>

class TextureManager : public ResourceManager<TextureManager, sf::Texture> {
public:
    TextureManager() : ResourceManager("textures.cfg") {};

    sf::Texture* Load(const std::string& l_path) {
        sf::Texture* texture = new sf::Texture();
        if (!texture->loadFromFile(Utils::GetWorkingDirectory() + l_path)) {
            std::cout << "Failed to load texture from file: " << l_path << std::endl;
            delete texture;
            texture = nullptr;
        }
        return texture;
    };
};