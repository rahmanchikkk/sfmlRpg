#include "State_Login.h"
#include "StateManager.h"

State_Login::State_Login(StateManager* l_stateMgr) : BaseState(l_stateMgr), m_client(m_stateMgr->GetContext()->m_client) {}

State_Login::~State_Login() {}

void State_Login::OnCreate() {
    m_client->SetPlayerName("login");
    m_client->Setup(&State_Login::HandlePacket, this);
    if (m_client->Connect()) {
        EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
        GUI_Manager* guiMgr = m_stateMgr->GetContext()->m_guiManager;
        guiMgr->LoadInterface(StateType::Login, "Login.interface", "Login");
        GUI_Interface* menu = guiMgr->GetInterface(StateType::Login, "Login");
        menu->SetPosition(sf::Vector2f(m_stateMgr->GetContext()->m_wind->GetWindowSize().x / 3.2f, 
            m_stateMgr->GetContext()->m_wind->GetWindowSize().y / 3.5f));
        evMgr->AddCallback(StateType::Login, "Login_proceed", &State_Login::Proceed, this);
    } else {
        std::cout << "Failed to connect to the game server.\n";
        m_stateMgr->Remove(StateType::Login);
        m_stateMgr->SwitchTo(StateType::MainMenu);
    }
}

void State_Login::OnDestroy() {
    m_client->Disconnect();
    m_client->UnregisterPacketHandler();
    EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
    GUI_Manager* guiMgr = m_stateMgr->GetContext()->m_guiManager;
    guiMgr->RemoveInterface(StateType::Login, "Login");
    evMgr->RemoveCallback(StateType::Login, "Login_proceed");
}

void State_Login::Activate() {
    GUI_Interface* menu = m_stateMgr->GetContext()->m_guiManager->GetInterface(StateType::Login, "Login");
    menu->GetElement("NameLabel")->SetActive(true);
    menu->GetElement("Nickname")->SetActive(true);
    menu->GetElement("PasswordLabel")->SetActive(true);
    menu->GetElement("Password")->SetActive(true);
    menu->GetElement("EmailLabel")->SetActive(true);
    menu->GetElement("Email")->SetActive(true);
    menu->GetElement("Proceed")->SetActive(true);
}

void State_Login::Proceed(EventDetails* l_details) {
    GUI_Interface* menu = m_stateMgr->GetContext()->m_guiManager->GetInterface(StateType::Login, "Login");
    std::string username = menu->GetElement("Nickname")->GetText();
    std::string userpwd = menu->GetElement("Password")->GetText();
    std::string useremail = menu->GetElement("Email")->GetText();
    std::string str = "SELECT * FROM client WHERE (nickname = '" + username + "' AND pwd = '" + userpwd + "' AND email = '" + useremail + "')";
    std::cout << str << std::endl;
    sf::Packet packet;
    StampPacket(PacketType::Login, packet);
    packet << str;
    m_client->Send(packet);
}

void State_Login::Update(const sf::Time& l_time) {}
void State_Login::Draw() {}
void State_Login::Deactivate() {}

void State_Login::HandlePacket(const PacketID& l_pid, sf::Packet& l_packet, Client* l_client) {
    PacketType pType = (PacketType)l_pid;
    if (pType == PacketType::Connect) {
        std::cout << "connect" << std::endl;
    } else if (pType == PacketType::Login) {
        std::cout << "Logged in successfully!\n";
        ClientData data;
        l_packet >> data;
        std::cout << "ID: " << data.m_id << "\n";
        std::cout << "Nickname: " << data.m_nickname << "\n";
        std::cout << "Password: " << data.m_password << "\n";
        std::cout << "Email: " << data.m_email << "\n";
        std::cout << "Gold: " << data.m_gold << "\n";
        std::cout << "Skins: ";
        for (int& i : data.m_skins) {
            std::cout << i << ' ';
        }
        std::cout << "\n";
        m_stateMgr->GetContext()->m_clientData = data;
        m_stateMgr->SwitchTo(StateType::MainMenu);
    } else if (pType == PacketType::Disconnect) {
        std::cout << "Failed to log in!\n";
    }
}