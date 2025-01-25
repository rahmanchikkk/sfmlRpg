#pragma once
#include "TextureManager.h"
#include "FontManager.h"
#include "EventManager.h"

struct SharedContext {
    SharedContext() : m_textureManager(nullptr), m_fontManager(nullptr), m_eventManager(nullptr) {};
    TextureManager* m_textureManager;
    FontManager* m_fontManager;
    EventManager* m_eventManager;
};