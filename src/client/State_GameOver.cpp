#include "State_GameOver.h"
#include "StateManager.h"

State_GameOver::State_GameOver(StateManager* l_stateMgr) : BaseState(l_stateMgr) {}

State_GameOver::~State_GameOver() {}

void State_GameOver::OnCreate() {
    m_elapsed = 0;
    m_text.setString(sf::String("Congratulations! You beat the game."));
    m_text.setCharacterSize(15);
    m_text.setColor(sf::Color::White);
    m_text.setOrigin(m_text.getLocalBounds().width / 2.0f, m_text.getLocalBounds().height / 2.0f);
    m_text.setPosition(400, 300);
    m_stateManager->Remove(StateType::Game);
}

void State_GameOver::OnDestroy() {}

void State_GameOver::Update(const sf::Time& l_time) {
    m_elapsed += l_time.asSeconds();
    if (m_elapsed >= 5.0f) {
        m_stateManager->Remove(StateType::GameOver);
        m_stateManager->SwitchTo(StateType::MainMenu);
    }
}

void State_GameOver::Draw() {
    sf::RenderWindow* wind = m_stateManager->GetContext()->m_window->GetRenderWindow();
    wind->draw(m_text);
}

void State_GameOver::Activate() {}
void State_GameOver::Deactivate() {}