#include "S_CharacterUI.h"
#include "SystemManager.h"

S_CharacterUI::S_CharacterUI(SystemManager* l_sysMgr) : S_Base(System::Character_UI, l_sysMgr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::Position);
    req.TurnOnBit((unsigned int)Component::UI_Element);
    req.TurnOnBit((unsigned int)Component::Health);
    m_requiredComponents.push_back(req);
    req.ClearBit((unsigned int)Component::Health);
    req.TurnOnBit((unsigned int)Component::Name);
    m_requiredComponents.push_back(req);
    ClientSystemManager* systemManager = (ClientSystemManager*)m_systemManager;
    systemManager->GetTextureManager()->RequireResource("HeartBar");
    systemManager->GetFontManager()->RequireResource("Main");
    sf::Texture* txtr = systemManager->GetTextureManager()->GetResource("HeartBar");
    txtr->setRepeated(true);
    m_heartbarSize = txtr->getSize();
    m_heartBar.setTexture(*txtr);
    m_heartBar.setScale(0.5f, 0.5f);
    m_heartBar.setOrigin(m_heartbarSize.x / 2, m_heartbarSize.y / 2);
    m_nickname.setFont(*systemManager->GetFontManager()->GetResource("Main"));
    m_nickname.setCharacterSize(9);
    m_nickname.setColor(sf::Color::White);
    m_nickname.setFillColor(sf::Color(100, 100, 100, 100));
}

S_CharacterUI::~S_CharacterUI() { 
    ClientSystemManager* mgr = (ClientSystemManager*)m_systemManager;
    mgr->GetTextureManager()->ReleaseResource("HeartBar");
    mgr->GetFontManager()->ReleaseResource("Main");
}

void S_CharacterUI::Update(float l_dT) {}
void S_CharacterUI::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event) {}
void S_CharacterUI::Notify(const Message& l_msg) {}

void S_CharacterUI::Render(Window* l_wind) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    for (auto& itr : m_entities) {
        C_Position* pos = entityManager->GetComponent<C_Position>(itr, Component::Position);
        C_UI_Element* ui = entityManager->GetComponent<C_UI_Element>(itr, Component::UI_Element);
        C_Health* health = entityManager->GetComponent<C_Health>(itr, Component::Health);
        C_Name* name = entityManager->GetComponent<C_Name>(itr, Component::Name);
        if (health) {
            m_heartBar.setTextureRect(sf::IntRect(0, 0, (health->GetHealth() * m_heartbarSize.x), m_heartbarSize.y));
            m_heartBar.setOrigin((health->GetHealth() * m_heartbarSize.x) / 2, m_heartbarSize.y / 2);
            m_heartBar.setPosition(pos->GetPosition() + ui->GetOffset());
            l_wind->GetRenderWindow()->draw(m_heartBar);
        }
        if (name) {
            m_nickname.setString(name->GetName());
            m_nickname.setOrigin(m_nickname.getLocalBounds().width / 2, m_nickname.getLocalBounds().width / 2);
            if (health) {
                m_nickname.setPosition(m_heartBar.getPosition().x, m_heartBar.getPosition().y - m_heartbarSize.y);
            } else {
                m_nickname.setPosition(pos->GetPosition() + ui->GetOffset());
            }
            m_nickBg.setSize(sf::Vector2f(m_nickBg.getGlobalBounds().width + 2, m_nickname.getCharacterSize() + 1));
            m_nickBg.setOrigin(m_nickBg.getSize().x / 2, m_nickBg.getSize().y / 2);
            m_nickBg.setPosition(m_nickname.getPosition().x + 1, m_nickname.getPosition().y + 1);
            l_wind->GetRenderWindow()->draw(m_nickBg);
            l_wind->GetRenderWindow()->draw(m_nickname);
        }
    }
}