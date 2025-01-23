#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(TextureManager* l_textureMgr) : m_textureManager(l_textureMgr), m_direction(Direction::Down), 
    m_animCurrent(nullptr), m_spriteScale(1.f, 1.f) {};

SpriteSheet::~SpriteSheet() {
    ReleaseSheet();
}

void SpriteSheet::ReleaseSheet() {
    m_animCurrent = nullptr;
    m_textureManager->ReleaseResource(m_texture);
    while (m_animations.begin() != m_animations.end()) {
        delete m_animations.begin()->second;
        m_animations.erase(m_animations.begin());
    }
}

void SpriteSheet::CropSprite(const sf::IntRect& l_rect) {
    m_sprite.setTextureRect(l_rect);
}

const sf::Vector2u& SpriteSheet::GetSpriteSize() const {
    return m_spriteSize;
}

const sf::Vector2f& SpriteSheet::GetSpritePosition() const {
    return m_sprite.getPosition();
}

void SpriteSheet::SetSpriteSize(const sf::Vector2u& l_vec) {
    m_spriteSize = l_vec;
    m_sprite.setOrigin(m_spriteSize.x / 2, m_spriteSize.y);
}

void SpriteSheet::SetSpritePosition(const sf::Vector2f& l_vec) {
    m_sprite.setPosition(l_vec);
}

void SpriteSheet::SetDirection(const Direction& l_direction) {
    if (m_direction == l_direction) return;
    m_direction = l_direction;
    m_animCurrent->CropSprite();
}

Direction SpriteSheet::GetDirection() { return m_direction; }

void SpriteSheet::SetSheetPadding(const sf::Vector2f& l_vec) {
    m_sheetPadding = l_vec;
}

void SpriteSheet::SetSpriteSpacing(const sf::Vector2f& l_vec) {
    m_spriteSpacing = l_vec;
}

const sf::Vector2f& SpriteSheet::GetSheetPadding() const {
    return m_sheetPadding;
}

const sf::Vector2f& SpriteSheet::GetSpriteSpacing() const {
    return m_spriteSpacing;
}

bool SpriteSheet::LoadSheet(const std::string& l_path) {
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + l_path);
    if (!file.is_open()) {
        std::cout << "Failed opening a spritesheet from file: " << l_path << std::endl;
        return false;
    }
    ReleaseSheet();
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') continue;
        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "Texture") {
            if (m_texture != "") {
                std::cout << "Duplicate texture!" << std::endl;
                continue;
            }
            std::string texture;
            keystream >> texture;
            if (!m_textureManager->RequireResource(texture)) {
                std::cout << "Unknown texture: " << texture << std::endl;
                continue;
            }
            m_texture = texture;
            m_sprite.setTexture(*m_textureManager->GetResource(m_texture));
        } else if (type == "Size") {
            keystream >> m_spriteSize.x >> m_spriteSize.y;
        } else if (type == "Scale") {
            keystream >> m_spriteScale.x >> m_spriteScale.y;
        } else if (type == "Padding") {
            keystream >> m_sheetPadding.x >> m_sheetPadding.y;
        } else if (type == "Spacing") {
            keystream >> m_spriteSpacing.x >> m_spriteSpacing.y;
        } else if (type == "AnimationType") {
            keystream >> m_animType;
        } else if (type == "Animation") {
            std::string name;
            keystream >> name;
            if (m_animations.find(name) != m_animations.end()) {
                std::cout << "Duplicate animation: " << name << std::endl;
                continue;
            }
            AnimBase* anim = nullptr;
            if (m_animType == "Directional") {
                anim = new Anim_Directional();
            } else {
                std::cout << "Unknown animation type: " << m_animType << std::endl;
                continue;
            }
            keystream >> *anim;
            anim->SetName(name);
            anim->SetSpriteSheet(this);
            anim->Reset();
            m_animations.emplace(name, anim);
            if (m_animCurrent) continue;
            m_animCurrent = anim;
            m_animCurrent->Play();
        }
    }
    file.close();
    return true;
}

AnimBase* SpriteSheet::GetCurrentAnimation() { return m_animCurrent; }

bool SpriteSheet::SetAnimation(const std::string& l_anim, bool l_loop, bool l_play) {
    auto itr = m_animations.find(l_anim);
    if (itr == m_animations.end()) return false;
    if (itr->second == m_animCurrent) return false;
    if (m_animCurrent) {
        m_animCurrent->Stop();
    }
    m_animCurrent = itr->second;
    m_animCurrent->SetLooping(l_loop);
    if (l_play) m_animCurrent->Play();
    m_animCurrent->CropSprite();
    return true;
}

void SpriteSheet::Update(float l_dT) {
    m_animCurrent->Update(l_dT);
}

void SpriteSheet::Draw(sf::RenderWindow* l_wind) {
    l_wind->draw(m_sprite);
}