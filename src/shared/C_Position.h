#pragma once
#include "C_Base.h"
#include <SFML/System/Vector2.hpp>

class C_Position : public C_Base {
public:
    C_Position() : C_Base(Component::Position) {};
    ~C_Position() {};

    void ReadIn(std::stringstream& l_stream) {
        l_stream >> m_position.x >> m_position.y >> m_elevation;
    };

    const sf::Vector2f& GetPosition() { return m_position; };
    const sf::Vector2f& GetOldPosition() { return m_position; };
    unsigned int GetElevation() { return m_elevation; };

    void SetPosition(const sf::Vector2f& l_vec) {
        m_positionOld = m_position;
        m_position = l_vec;
    };

    void SetPosition(float l_x, float l_y) {
        m_positionOld = m_position;
        m_position = sf::Vector2f(l_x, l_y);
    };

    void SetElevation(unsigned int l_elevation) { m_elevation = l_elevation; };

    void MoveBy(const sf::Vector2f& l_vec) {
        m_positionOld = m_position;
        m_position += l_vec;
    };

    void MoveBy(float l_x, float l_y) {
        m_positionOld = m_position;
        m_position += sf::Vector2f(l_x, l_y);
    };
private:
    sf::Vector2f m_positionOld;
    sf::Vector2f m_position;
    unsigned int m_elevation;
};