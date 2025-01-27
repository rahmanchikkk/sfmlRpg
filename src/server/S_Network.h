#pragma once
#include "S_Base.h"
#include "ServerEntityManager.h"
#include "C_Client.h"
#include "S_State.h"
#include "S_Movement.h"
#include "NetworkDefinitions.h"
#include "Server.h"


struct PlayerInput {
    PlayerInput(): m_movedX(0), m_movedY(0), m_attacking(false) {};
    int m_movedX;
    int m_movedY;
    bool m_attacking;
};

using PlayerInputContainer = std::unordered_map<EntityId, PlayerInput>;

class S_Network : public S_Base {
public:
    S_Network(SystemManager* l_sysMgr);
    ~S_Network();

    void Update(float l_dT);
    void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
    void Notify(const Message& l_msg);

    bool RegisterClientID(const EntityId& l_entity, const ClientID& l_client);
    void RegisterServer(Server* l_server);
    ClientID GetClientID(const EntityId& l_entity);
    EntityId GetEntityId(const ClientID& l_client);

    void CreateSnapshot(sf::Packet& l_packet);
    void UpdatePlayer(sf::Packet& l_packet, const ClientID& l_client);
private:
    PlayerInputContainer m_playerInput;
    Server* m_server;
};