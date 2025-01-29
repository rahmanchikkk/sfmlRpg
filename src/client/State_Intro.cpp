#include "State_Intro.h"
#include "StateManager.h"

State_Intro::State_Intro(StateManager* l_stateMgr) : BaseState(l_stateMgr) {}

State_Intro::~State_Intro() {}

void State_Intro::OnCreate() {
    sf::Vector2u windowSize = m_stateManager->GetContext()->m_window->GetWindowSize();
    TextureManager* textureManager = m_stateManager->GetContext()->m_textureManager;
    textureManager->RequireResource("Intro");
    m_introSprite.setTexture(*textureManager->GetResource("Intro"));
    m_introSprite.setOrigin(textureManager->GetResource("Intro")->getSize().x / 2.0f,
        textureManager->GetResource("Intro")->getSize().y / 2.0f);
    m_introSprite.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);
    m_text.setString(sf::String("press space to coninue"));
    m_text.setCharacterSize(15);
    sf::FloatRect textureRect = m_text.getLocalBounds();
    m_text.setOrigin(textureRect.left + textureRect.width / 2.0f, 
        textureRect.left + textureRect.height / 2.0f);
    m_text.setPosition(m_introSprite.getPosition().x, m_introSprite.getPosition().y + 
        (textureManager->GetResource("Intro")->getSize().y / 1.5f));
    EventManager* evMgr = m_stateManager->GetContext()->m_eventManager;
    evMgr->AddCallback(StateType::Intro, "Intro_Continue", &State_Intro::Continue, this);
    m_stateManager->GetContext()->m_soundManager->PlayMusic("Electrix", 100.f, true);
}

void State_Intro::OnDestroy() {
    TextureManager* textureManager = m_stateManager->GetContext()->m_textureManager;
    textureManager->ReleaseResource("Intro");
    EventManager* evMgr = m_stateManager->GetContext()->m_eventManager;
    evMgr->RemoveCallback(StateType::Intro, "Intro_Continue");
}

void State_Intro::Continue(EventDetails* l_details) {
    m_stateManager->Remove(StateType::Intro);
    m_stateManager->SwitchTo(StateType::MainMenu);
}

void State_Intro::Draw() {
    m_stateManager->GetContext()->m_window->GetRenderWindow()->draw(m_introSprite);
    m_stateManager->GetContext()->m_window->GetRenderWindow()->draw(m_text);
}

void State_Intro::Update(const sf::Time& l_time) {}
void State_Intro::Activate() {}
void State_Intro::Deactivate() {}