#include "State_Paused.h"
#include "StateManager.h"

State_Paused::State_Paused(StateManager* l_stateMgr) : BaseState(l_stateMgr) {}

State_Paused::~State_Paused() {}

void State_Paused::OnCreate() {
    SetTransparent(true);
    m_text.setString(sf::String("PAUSED"));
    m_text.setCharacterSize(15);
    m_text.setStyle(sf::Text::Bold);

    sf::Vector2u windowSize = m_stateManager->GetContext()->m_window->GetWindowSize();
    sf::FloatRect rect = m_text.getLocalBounds();
    m_text.setOrigin(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
    m_text.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);
    m_rect.setSize(sf::Vector2f(windowSize));
    m_rect.setPosition(0.f, 0.f);
    m_rect.setFillColor(sf::Color(0, 0, 0, 150));
    EventManager* evMgr = m_stateManager->GetContext()->m_eventManager;
    evMgr->AddCallback(StateType::Paused, "Key_P", &State_Paused::Unpause, this);
}

void State_Paused::OnDestroy() {
    EventManager* evMgr = m_stateManager->GetContext()->m_eventManager;
    evMgr->RemoveCallback(StateType::Paused, "Key_P");
}

void State_Paused::Unpause(EventDetails* l_details) {
    m_stateManager->SwitchTo(StateType::Game);
}

void State_Paused::Draw() {
    sf::RenderWindow* wind = m_stateManager->GetContext()->m_window->GetRenderWindow();
    wind->draw(m_rect);
    wind->draw(m_text);
}

void State_Paused::Update(const sf::Time& l_time) {}
void State_Paused::Activate() {}
void State_Paused::Deactivate() {}