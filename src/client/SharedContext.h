#pragma once
#include "ClientEntityManager.h"
#include "TextureManager.h"
#include "FontManager.h"
#include "EventManager.h"
#include "Window.h"
#include "GUI_Manager.h"
#include "SoundManager.h"
#include "AudioManager.h"
#include "DebugOverlay.h"
#include "Client.h"
#include "ClientSystemManager.h"

class Map;

struct SharedContext {
    SharedContext() : m_textureManager(nullptr), m_fontManager(nullptr), m_eventManager(nullptr), m_window(nullptr),
        m_gameMap(nullptr), m_guiManager(nullptr), m_client(nullptr), m_audioManager(nullptr), m_soundManager(nullptr),
        m_entityManager(nullptr), m_debugOverlay(nullptr), m_systemManager(nullptr) {};
    TextureManager* m_textureManager;
    FontManager* m_fontManager;
    EventManager* m_eventManager;
    Window* m_window;
    Map* m_gameMap;
    GUI_Manager* m_guiManager;
    Client* m_client;
    AudioManager* m_audioManager;
    SoundManager* m_soundManager;
    ClientEntityManager* m_entityManager;
    DebugOverlay* m_debugOverlay;
    ClientSystemManager* m_systemManager;
};