#pragma once
#include "ResourceManager.h"
#include <SFML/Graphics/Font.hpp>

class FontManager : public ResourceManager<FontManager, sf::Font> {
public:
    FontManager() : ResourceManager("fonts.cfg") {};

    sf::Font* Load(const std::string& l_path) {
        sf::Font* font = new sf::Font();
        if (!font->loadFromFile(Utils::GetWorkingDirectory() + l_path)) {
            std::cout << "Failed to load font from file: " << l_path << std::endl;
            delete font;
            font = nullptr;
        }
        return font;
    };
};