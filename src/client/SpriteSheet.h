#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "TextureManager.h"
#include "Utilities.h"
#include "Anim_Directional.h"
#include <unordered_map>
#include "Directions.h"

using Animations = std::unordered_map<std::string, AnimBase*>;

class SpriteSheet {
public:
    SpriteSheet(TextureManager* m_textureMgr);
    ~SpriteSheet();

    void CropSprite(const sf::IntRect& l_rect);
    const sf::Vector2u& GetSpriteSize() const;
    const sf::Vector2f& GetSpritePosition() const;

    void SetSpriteSize(const sf::Vector2u& l_vec);
    void SetSpritePosition(const sf::Vector2f& l_vec);

    void SetDirection(const Direction& l_direction);
    Direction GetDirection();

    void SetSheetPadding(const sf::Vector2f& l_vec);
    void SetSpriteSpacing(const sf::Vector2f& l_vec);
    const sf::Vector2f& GetSheetPadding() const;
    const sf::Vector2f& GetSpriteSpacing() const;

    bool LoadSheet(const std::string& l_file);
    void ReleaseSheet();
    AnimBase* GetCurrentAnimation();

    bool SetAnimation(const std::string& l_animation, bool l_loop = false, bool l_play = false);
    void Update(float l_dT);
    void Draw(sf::RenderWindow* l_wind);
private:
    AnimBase* m_animCurrent;
    Animations m_animations;
    TextureManager* m_textureManager;
    
    sf::Vector2f m_sheetPadding;
    sf::Vector2f m_spriteSpacing;
    sf::Vector2u m_spriteSize;
    sf::Vector2f m_spriteScale;

    std::string m_texture;
    std::string m_animType;
    Direction m_direction;
    sf::Sprite m_sprite;
};