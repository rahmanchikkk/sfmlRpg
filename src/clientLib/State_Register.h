#pragma once
#include "BaseState.h"
#include "EventManager.h"
#include "Client.h"

class State_Register : public BaseState {
public:
    State_Register(StateManager* l_stateMgr);
    ~State_Register();

    void OnCreate();
    void OnDestroy();

    void Activate();
    void Deactivate();

    void Update(const sf::Time& l_time);
    void Draw();

    void Proceed(EventDetails* l_details);
    void HandlePacket(const PacketID& l_pid, sf::Packet& l_packet, Client* l_client);
private:
    Client* m_client;
};