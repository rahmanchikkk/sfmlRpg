#pragma once
#include "S_Base.h"
#include "C_UI_Element.h"
#include "C_Name.h"
#include "C_Health.h"
#include "Window.h"
#include "ClientSystemManager.h"

class S_CharacterUI : public S_Base {
public:
    S_CharacterUI(SystemManager* l_sysMgr);
    ~S_CharacterUI();

    void Update(float l_dT);
    void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
    void Notify(const Message& l_msg);

    void Render(Window* l_wind);
private:
    sf::Sprite m_heartBar;
    sf::RectangleShape m_nickBg;
    sf::Text m_nickname;
    sf::Vector2u m_heartbarSize;
};