#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class DebugOverlay {
public:
    DebugOverlay() {
        m_debug = false;
    }

    ~DebugOverlay() {
        for (auto& itr : m_drawables) {
            delete itr;
        }
    }

    void Add(sf::Drawable* l_drawable) {
        m_drawables.push_back(l_drawable);
    }

    void Draw(sf::RenderWindow* l_wind) {
        while (m_drawables.begin() != m_drawables.end()) {
            l_wind->draw(**m_drawables.begin());
            delete *m_drawables.begin();
            m_drawables.erase(m_drawables.begin());
        }
    }

    bool Debug() { return m_debug; }
    void SetDebug(const bool& l_val) { m_debug = l_val; }
private:
    std::vector<sf::Drawable*> m_drawables;
    bool m_debug;
};