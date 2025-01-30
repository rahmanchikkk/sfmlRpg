#pragma once
#include "Window.h"
#include "EventManager.h"
#include "TextureManager.h"
#include "StateManager.h"
#include <iostream>

class Game {
public:
    Game();
    ~Game();

    void Update();
    void Render();
    void LateUpdate();

    sf::Time GetElapsed();
    Window* GetWindow();
private:
    void RestartClock();
    
    sf::Clock m_clock;
    sf::Time m_elapsed;
    SharedContext m_context;
    ClientEntityManager m_entityManager;
    ClientSystemManager m_systemManager;
    StateManager m_stateManager;
    Client m_client;
    TextureManager m_textureManager;
    FontManager m_fontManager;
    AudioManager m_audioManager;
    SoundManager m_soundManager;
    GUI_Manager m_guiManager;
    EventManager m_eventManager;
    Window m_window;
    DebugOverlay m_debugOverlay;
};