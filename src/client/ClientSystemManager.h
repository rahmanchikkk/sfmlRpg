#pragma once
#include "SystemManager.h"
#include "Window.h"
#include "S_State.h"
#include "S_Control.h"
#include "S_Renderer.h"
#include "S_SheetAnimation.h"
#include "S_CharacterUI.h"
#include "S_Collision.h"
#include "S_Movement.h"
#include "S_Network.h"
#include "S_Sound.h"
#include "FontManager.h"
#include "DebugOverlay.h"

class ClientSystemManager : public SystemManager {
public:
    ClientSystemManager(TextureManager* l_textureMgr, FontManager* l_fontMgr);
    ~ClientSystemManager();

    TextureManager* GetTextureManager();
    FontManager* GetFontManager();

    void Draw(Window* l_wind, unsigned int l_elevation);
    DebugOverlay* m_debugOverlay;
private:
    TextureManager* m_textureManager;
    FontManager* m_fontManager;
};