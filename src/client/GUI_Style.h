#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

enum class GUI_ElementState { Neutral, Focused, Clicked };

struct GUI_Style {
    GUI_Style() : m_textSize(12), m_textCenterOrigin(false), m_backgroundImageColor(255, 255, 255, 255) {
        sf::Color none = sf::Color(0, 0, 0, 0);
        m_elementColor = none;
        m_textColor = none;
        m_backgroundColor = none;
    };

    sf::Vector2f m_size;
    sf::Color m_backgroundColor;
    sf::Color m_elementColor;
    sf::Color m_backgroundImageColor;
    std::string m_backgroundImage;
    sf::Color m_textColor;
    std::string m_textFont;
    unsigned int m_textSize;
    sf::Vector2f m_textPadding;
    bool m_textCenterOrigin;
    std::string m_glyph;
    sf::Vector2f m_glyphPadding;
};

struct GUI_Visual {
    sf::RectangleShape m_backgroundSolid;
    sf::Sprite m_backgroundImage;
    sf::Sprite m_glyph;
    sf::Text m_text;
};