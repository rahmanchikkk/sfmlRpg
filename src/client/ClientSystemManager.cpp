#include "ClientSystemManager.h"

ClientSystemManager::ClientSystemManager(TextureManager* l_textureMgr, FontManager* l_fontMgr) : 
    m_textureManager(l_textureMgr), m_fontManager(l_fontMgr) {
    AddSystem<S_State>(System::State);
    AddSystem<S_Control>(System::Control);
    AddSystem<S_Movement>(System::Movement);
    AddSystem<S_Collision>(System::Collision);
    AddSystem<S_Network>(System::Network);
    AddSystem<S_CharacterUI>(System::Character_UI);
    AddSystem<S_SheetAnimation>(System::SheetAnimation);
    AddSystem<S_Renderer>(System::Renderer);
    AddSystem<S_Sound>(System::Sound);       
}

ClientSystemManager::~ClientSystemManager() {}

TextureManager* ClientSystemManager::GetTextureManager() { return m_textureManager; }
FontManager* ClientSystemManager::GetFontManager() { return m_fontManager; }

void ClientSystemManager::Draw(Window* l_wind, unsigned int l_elevation) {
    auto itr = m_systems.find(System::Renderer);
    if (itr != m_systems.end()) {
        S_Renderer* renderer = (S_Renderer*)itr->second;
        renderer->Render(l_wind, l_elevation);
    }
    auto i = m_systems.find(System::Character_UI);
    if (i != m_systems.end()) {
        S_CharacterUI* ui = (S_CharacterUI*)i->second;
        ui->Render(l_wind);
    }
}