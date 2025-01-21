#pragma once
#include "C_Base.h"
#include <SFML/Graphics/Rect.hpp>

enum class Origin { Top_Left, Abs_Center, Mid_Bottom };

class C_Collidable : public C_Base {
public:
    C_Collidable() : C_Base(Component::Collidable), m_collidingOnX(false), m_collidingOnY(false), m_origin(Origin::Mid_Bottom) {};
    void ReadIn(std::stringstream& l_stream) {
        unsigned int origin = 0;
        l_stream >> m_AABB.width >> m_AABB.height >> m_offset.x >> m_offset.y >> origin;
        m_origin = (Origin)origin;
    };

    const sf::FloatRect& GetCollidable() { return m_AABB; };
    bool IsColldiingX() { return m_collidingOnX; };
    bool IsCollidingY() { return m_collidingOnY; };
    void CollideOnX() { m_collidingOnX = true; };
    void CollideOnY() { m_collidingOnY = true; };

    void SetOrigin(const Origin& l_origin) { m_origin = l_origin; };
    void SetSize(const sf::Vector2f& l_vec) {
        m_AABB.width = l_vec.x;
        m_AABB.height = l_vec.y;
    };

    void SetCollidable(const sf::FloatRect& l_rect) { m_AABB = l_rect; };
    void ResetCollisionFlags() {
        m_collidingOnX = false;
        m_collidingOnY = false;
    };

    void SetPosition(const sf::Vector2f& l_position) {
        switch (m_origin) {
        case Origin::Top_Left:
            m_AABB.left = l_position.x + m_offset.x;
            m_AABB.top = l_position.y + m_offset.y;
            break;
        case Origin::Abs_Center:
            m_AABB.left = l_position.x - (m_AABB.width / 2)  + m_offset.x;
            m_AABB.top = l_position.y - (m_AABB.height / 2) + m_offset.y;
            break;
        case Origin::Mid_Bottom:
            m_AABB.left = l_position.x - (m_AABB.width / 2) + m_offset.x;
            m_AABB.top = l_position.y - m_AABB.height + m_offset.y;
            break;
        }
    };
private:
    sf::FloatRect m_AABB;
    bool m_collidingOnX;
    bool m_collidingOnY;
    Origin m_origin;
    sf::Vector2f m_offset;
};