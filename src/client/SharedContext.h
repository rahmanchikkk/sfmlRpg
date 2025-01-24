#pragma once
#include "TextureManager.h"
#include "FontManager.h"

struct SharedContext {
    SharedContext() : m_textureManager(nullptr), m_fontManager(nullptr) {};
    TextureManager* m_textureManager;
    FontManager* m_fontManager;
};