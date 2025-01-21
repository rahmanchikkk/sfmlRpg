#pragma once
#include "C_Base.h"
#include "Directions.h"
#include <SFML/System/Vector2.hpp>

class C_Movable : public C_Base {
public:
    C_Movable() : C_Base(Component::Movable), m_velocityMax(0.f), m_direction(Direction::Down) {};
    void ReadIn(std::stringstream& l_stream) {
        unsigned int dir = 0;
        l_stream >> m_velocityMax >> m_speed.x >> m_speed.y >> dir;
        m_direction = (Direction)dir;
    };

    const sf::Vector2f& GetVelocity() { return m_velocity; };
    const sf::Vector2f& GetAcceleration() { return m_acceleration; };
    const sf::Vector2f& GetSpeed() { return m_speed; };
    float GetMaxVelocity() { return m_velocityMax; };
    Direction GetDirection() { return m_direction; };

    void SetVelocity(const sf::Vector2f& l_vec) { m_velocity = l_vec; };
    void SetAccelerstion(const sf::Vector2f& l_vec) { m_acceleration = l_vec; };
    void SetSpeed(const sf::Vector2f& l_vec) { m_speed = l_vec; };
    void SetMaxVelocity(float l_max) { m_velocityMax = l_max; };
    void SetDirection(const Direction& l_dir) { m_direction = l_dir; };

    void AddVelocity(const sf::Vector2f& l_vec) {
        m_velocity += l_vec;
        if (std::abs(m_velocity.x) > m_velocityMax) {
            m_velocity.x = m_velocityMax * (m_velocity.x / std::abs(m_velocity.x));
        }
        if (std::abs(m_velocity.y) > m_velocityMax) {
            m_velocity.y = m_velocityMax * (m_velocity.y / std::abs(m_velocity.y));
        }
    };

    void ApplyFriction(const sf::Vector2f& l_vec) {
        if (std::abs(l_vec.x) - std::abs(m_velocity.x) > 0) {
            m_velocity.x = 0;
        } else {
            m_velocity.x += l_vec.x * (m_velocity.x > 0 ? -1 : 1);
        }
        
        if (std::abs(l_vec.y) - std::abs(m_velocity.y) > 0) {
            m_velocity.y = 0;
        } else {
            m_velocity.y += l_vec.y * (m_velocity.y > 0 ? -1 : 1);
        }
    };

    void Accelerate(const sf::Vector2f& l_vec) { m_acceleration += l_vec; };
    void Accelerate(float l_x, float l_y) { m_acceleration += sf::Vector2f(l_x, l_y); };

    void Move(const Direction& l_direction) {
        if (l_direction == Direction::Up) {
            m_acceleration.y -= m_speed.y;
        } else if (l_direction == Direction::Down) {
            m_acceleration.y += m_speed.y;
        } else if (l_direction == Direction::Left) {
            m_acceleration.x -= m_speed.x;
        } else if (l_direction == Direction::Right) {
            m_acceleration.x += m_speed.x;
        }
    };
private:
    sf::Vector2f m_speed;
    sf::Vector2f m_velocity;
    sf::Vector2f m_acceleration;
    float m_velocityMax;
    Direction m_direction;
};