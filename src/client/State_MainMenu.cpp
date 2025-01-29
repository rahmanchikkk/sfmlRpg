#include "State_MainMenu.h"
#include "StateManager.h"

State_MainMenu::State_MainMenu(StateManager* l_stateMgr) : BaseState(l_stateMgr) {}

State_MainMenu::~State_MainMenu() {}

void State_MainMenu::OnCreate() {
    SetTransparent(true);
    SetTranscedent(true);
    GUI_Manager* gui = m_stateManager->GetContext()->m_guiManager;
    gui->LoadInterface(StateType::MainMenu, "MainMenu.interface", "MainMenu");
    sf::Vector2u windowSize = m_stateManager->GetContext()->m_window->GetWindowSize();
    gui->GetInterface(StateType::MainMenu, "MainMenu")->SetPosition(sf::Vector2f(windowSize.x / 3.2f, windowSize.y / 3.5f));
    EventManager* evMgr = m_stateManager->GetContext()->m_eventManager;
    evMgr->AddCallback(StateType::MainMenu, "MainMenu_Play", &State_MainMenu::Play, this);
    evMgr->AddCallback(StateType::MainMenu, "MainMenu_Disconnect", &State_MainMenu::Disconnect, this);
    evMgr->AddCallback(StateType::MainMenu, "MainMenu_Quit", &State_MainMenu::Quit, this);
}

void State_MainMenu::OnDestroy() {
    m_stateManager->GetContext()->m_guiManager->RemoveInterface(StateType::MainMenu, "MainMenu");
    EventManager* evMgr = m_stateManager->GetContext()->m_eventManager;
    evMgr->RemoveCallback(StateType::MainMenu, "MainMenu_Play");
    evMgr->RemoveCallback(StateType::MainMenu, "MainMenu_Disconnect");
    evMgr->RemoveCallback(StateType::MainMenu, "MainMenu_Quit");
}

void State_MainMenu::Update(const sf::Time& l_time) {}
void State_MainMenu::Deactivate() {}
void State_MainMenu::Draw() {}

void State_MainMenu::Activate() {
    GUI_Interface* menu = m_stateManager->GetContext()->m_guiManager->GetInterface(StateType::MainMenu, "MainMenu");
    if (m_stateManager->HasState(StateType::Game)) {
        menu->GetElement("Play")->SetText("Resume");
        menu->GetElement("Disconnect")->SetActive(true);
        menu->GetElement("IP")->SetActive(false);
        menu->GetElement("PORT")->SetActive(false);
        menu->GetElement("IpLabel")->SetActive(false);
        menu->GetElement("PortLabel")->SetActive(false);
        menu->GetElement("NameLabel")->SetActive(false);
        menu->GetElement("Nickname")->SetActive(false);
    } else {
        menu->GetElement("Play")->SetText("Connect");
        menu->GetElement("Disconnect")->SetActive(false);
        menu->GetElement("IP")->SetActive(true);
        menu->GetElement("PORT")->SetActive(true);
        menu->GetElement("IpLabel")->SetActive(true);
        menu->GetElement("PortLabel")->SetActive(true);
        menu->GetElement("NameLabel")->SetActive(true);
        menu->GetElement("Nickname")->SetActive(true);
    }
}

void State_MainMenu::Play(EventDetails* l_details) {
    if (m_stateManager->HasState(StateType::Game)) {
        GUI_Interface* menu = m_stateManager->GetContext()->m_guiManager->GetInterface(StateType::MainMenu, "MainMenu");
        sf::IpAddress ip = menu->GetElement("IP")->GetText();
        PortNumber port = std::atoi(menu->GetElement("PORT")->GetText().c_str());
        std::string nickname = menu->GetElement("Nickname")->GetText();
        m_stateManager->GetContext()->m_client->SetServerInformation(ip, port);
        m_stateManager->GetContext()->m_client->SetPlayerName(nickname);
    }
    m_stateManager->SwitchTo(StateType::Game);
}

void State_MainMenu::Disconnect(EventDetails* l_details) {
    m_stateManager->GetContext()->m_client->Disconnect();
}

void State_MainMenu::Quit(EventDetails* l_details) {
    m_stateManager->GetContext()->m_window->Close();
}