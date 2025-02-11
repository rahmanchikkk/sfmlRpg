#pragma once
#include "BaseState.h"
#include "EventManager.h"
#include "Client.h"

class State_Login : public BaseState {
public:
    State_Login(StateManager* l_stateMgr);
    ~State_Login();

    void OnCreate();
    void OnDestroy();

    void Activate();
    void Deactivate();

    void Update(const sf::Time& l_time);
    void Draw();

    void HandlePacket(const PacketID& l_pid, sf::Packet& l_packet, Client* l_client);
    void Proceed(EventDetails* l_details);
private:
    Client* m_client;
};