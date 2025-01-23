#pragma once
#include "C_Base.h"
#include <SFML/System/Vector2.hpp>

class C_UI_Element : public C_Base {
public:
    C_UI_Element() : C_Base(Component::UI_Element), m_showName(false), m_showHealth(false) {};

    void ReadIn(std::stringstream& l_stream) {
        l_stream >> m_offset.x >> m_offset.y;
    };

    bool ShowName() { return m_showName; };
    void SetShowName(bool l_show) { m_showName = l_show; };

    bool ShowHealth() { return m_showHealth; };
    void SetShowHealth(bool l_show) { m_showName = l_show; };

    const sf::Vector2f& GetOffset() { return m_offset; };
    void SetOffset(const sf::Vector2f& l_vec) { m_offset = l_vec; };
private:
    sf::Vector2f m_offset;
    bool m_showName;
    bool m_showHealth;
};