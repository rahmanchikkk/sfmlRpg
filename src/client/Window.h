#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "EventManager.h"
#include <string>

class Window {
public:
    Window();
    Window(const std::string& l_title, const sf::Vector2u& l_windSize);
    ~Window();

    bool IsDone();
    bool IsFullscreen();
    bool IsFocused();

    void BeginDraw();
    void EndDraw();
    void Update();

    void ToggleFullscreen(EventDetails* l_details);
    void Close(EventDetails* l_details = nullptr);

    sf::RenderWindow* GetRenderWindow();
    EventManager* GetEventManager();
    sf::Vector2u GetWindowSize();
    sf::FloatRect GetViewSpace();
private:
    void Setup(const std::string& l_title, const sf::Vector2u& l_windSize);
    void Create();

    sf::RenderWindow m_window;
    EventManager m_eventManager;
    std::string m_title;
    sf::Vector2u m_windowSize;
    bool m_isDone;
    bool m_isFullscreen;
    bool m_isFocused;
};