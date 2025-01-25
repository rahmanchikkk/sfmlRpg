#pragma once
#include "TextureManager.h"
#include "FontManager.h"
#include "EventManager.h"
#include "Window.h"

struct SharedContext {
    SharedContext() : m_textureManager(nullptr), m_fontManager(nullptr), m_eventManager(nullptr), m_window(nullptr) {};
    TextureManager* m_textureManager;
    FontManager* m_fontManager;
    EventManager* m_eventManager;
    Window* m_window;
};