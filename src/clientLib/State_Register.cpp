#include "State_Register.h"
#include "StateManager.h"

State_Register::State_Register(StateManager* l_stateMgr) : BaseState(l_stateMgr) {}

State_Register::~State_Register() {}

void State_Register::OnCreate() {
    m_client->SetPlayerName("register");
    m_client->Setup(&State_Register::HandlePacket, this);
    if (m_client->Connect()) {
        EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
        GUI_Manager* guiMgr = m_stateMgr->GetContext()->m_guiManager;
        evMgr->AddCallback(StateType::Register, "Register_Proceed", &State_Register::Proceed, this);
        guiMgr->LoadInterface(StateType::Register, "Register.interface", "Register");
    } else {
        std::cout << "Failed connecting to server.\n";
    }
}

void State_Register::OnDestroy() {
    m_client->Disconnect();
    m_client->UnregisterPacketHandler();
    EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
    evMgr->RemoveCallback(StateType::Register, "Register_Proceed");
    GUI_Manager* guiMgr = m_stateMgr->GetContext()->m_guiManager;
    guiMgr->RemoveInterface(StateType::Register, "Register");
}

void State_Register::Activate() {
    GUI_Interface* menu = m_stateMgr->GetContext()->m_guiManager->GetInterface(StateType::Register, "Register");
    menu->GetElement("NameLabel")->SetActive(true);
    menu->GetElement("Nickname")->SetActive(true);
    menu->GetElement("PasswordLabel")->SetActive(true);
    menu->GetElement("Password")->SetActive(true);
    menu->GetElement("PwdRepeatLabel")->SetActive(true);
    menu->GetElement("PwdRepeat")->SetActive(true);
    menu->GetElement("EmailLabel")->SetActive(true);
    menu->GetElement("Email")->SetActive(true);
    menu->GetElement("Proceed")->SetActive(true);
}

void State_Register::Update(const sf::Time& l_time) {}
void State_Register::Draw() {}
void State_Register::Deactivate() {}

void State_Register::Proceed(EventDetails* l_details) {
    GUI_Interface* menu = m_stateMgr->GetContext()->m_guiManager->GetInterface(StateType::Register, "Register");
    std::string nickname = menu->GetElement("Nickname")->GetText();
    std::string password = menu->GetElement("Password")->GetText();
    std::string repeatPassword = menu->GetElement("PwdRepeat")->GetText();
    std::string email = menu->GetElement("Email")->GetText();
    if (password != repeatPassword) {
        std::cout << "Repeated password is not equal to password!\n";
        return;
    }
    std::string str = "SELECT * FROM client WHERE (nickname ='" + nickname + "' AND pwd = '" + password + "' AND email = '" + email + "')";
    sf::Packet packet;
    StampPacket(PacketType::Register, packet);
    packet << str;
    m_client->Send(packet);
}

void State_Register::HandlePacket(const PacketID& l_pid, sf::Packet& l_packet, Client* l_client) {
    PacketType pType = (PacketType)l_pid;
    if (pType == PacketType::Connect) {
        std::cout << "connect.\n";
    } else if (pType == PacketType::Register) {
        //
    } else if (pType == PacketType::Disconnect) {
        std::cout << "failed to sign up.\n";
    }
}