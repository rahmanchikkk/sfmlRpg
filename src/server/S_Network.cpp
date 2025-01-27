#include "S_Network.h"
#include "SystemManager.h"

S_Network::S_Network(SystemManager* l_sysMgr) : S_Base(System::Network, l_sysMgr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::Client);
    m_requiredComponents.push_back(req);
    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Hurt, this);
    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Removed_Entity, this);
    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Respawn, this);
}

S_Network::~S_Network() {}

void S_Network::Update(float l_dT) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    for (auto& entity : m_entities) {
        auto& input = m_playerInput[entity];
        if (input.m_movedX) {
            Message msg((MessageType)EntityMessage::Move);
            msg.m_receiver = entity;
            if (input.m_movedX < 0) msg.m_int = (int)Direction::Left;
            else msg.m_int = (int)Direction::Right;
            m_systemManager->GetMessageHandler()->Dispatch(msg);
        }
        if (input.m_movedY) {
            Message msg((MessageType)EntityMessage::Move);
            msg.m_receiver = entity;
            if (input.m_movedY < 0) msg.m_int = (int)Direction::Up;
            else msg.m_int = (int)Direction::Down;
            m_systemManager->GetMessageHandler()->Dispatch(msg);
        }
        if (input.m_attacking) {
            Message msg((MessageType)EntityMessage::Attack);
            msg.m_receiver = entity;
            m_systemManager->GetMessageHandler()->Dispatch(msg);
        }
    }
}

void S_Network::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event) {}

void S_Network::Notify(const Message& l_msg) {
    if (!HasEntity(l_msg.m_receiver)) return;
    EntityMessage m = (EntityMessage)l_msg.m_type;
    if (m == EntityMessage::Hurt) {
        sf::Packet packet;
        StampPacket(PacketType::Hurt, packet);
        packet << l_msg.m_receiver;
        m_server->Broadcast(packet);
        return;
    }
    if (m == EntityMessage::Removed_Entity) {
        m_playerInput.erase(l_msg.m_receiver);
        return;
    }
    if (m == EntityMessage::Respawn) {
        C_Position* pos = m_systemManager->GetEntityManager()->GetComponent<C_Position>(l_msg.m_receiver, Component::Position);
        if (!pos) return;
        pos->SetPosition(64.f, 64.f);
        pos->SetElevation(1);
    }
}

bool S_Network::RegisterClientID(const EntityId& l_entity, const ClientID& l_client) {
    if (!HasEntity(l_entity)) return false;
    m_systemManager->GetEntityManager()->GetComponent<C_Client>(l_entity, Component::Client)->SetClient(l_client);
    return true;
}

void S_Network::RegisterServer(Server* l_server) { 
    m_server = l_server;
}


ClientID S_Network::GetClientID(const EntityId& l_entity) {
    if (!HasEntity(l_entity)) return (ClientID)Network::NullID;
    return m_systemManager->GetEntityManager()->GetComponent<C_Client>(l_entity, Component::Client)->GetClient();
}

EntityId S_Network::GetEntityId(const ClientID& l_client) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    auto itr = std::find_if(m_entities.begin(), m_entities.end(), [&entityManager, &l_client](const EntityId& l_entity) {
        return entityManager->GetComponent<C_Client>(l_entity, Component::Client)->GetClient() == l_client;
    });
    return (itr != m_entities.end() ? *itr : (EntityId)Network::NullID);
}

void S_Network::CreateSnapshot(sf::Packet& l_packet) {
    sf::Lock lock(m_server->GetMutex());
    StampPacket(PacketType::Snapshot, l_packet);
    ServerEntityManager* entityManager = (ServerEntityManager*)m_systemManager->GetEntityManager();
    l_packet << entityManager->GetEntityCount();
    if (entityManager->GetEntityCount() > 0) {
        entityManager->DumpEntityInfo(l_packet);
    }
}

void S_Network::UpdatePlayer(sf::Packet& l_packet, const ClientID& l_client) {
    sf::Lock lock(m_server->GetMutex());
    EntityId eid = GetEntityId(l_client);
    if (eid < 0) return;
    if (!HasEntity(eid)) return;
    sf::Int8 entityMessage;
    m_playerInput[eid].m_attacking = false;
    while (l_packet >> entityMessage) {
        EntityMessage m = (EntityMessage)entityMessage;
        switch (m) {
        case EntityMessage::Move:
            sf::Int32 x, y;
            l_packet >> x >> y;
            m_playerInput[eid].m_movedX = x;
            m_playerInput[eid].m_movedY = y;
            break;
        case EntityMessage::Attack:
            sf::Int8 attackState;
            l_packet >> attackState;
            m_playerInput[eid].m_attacking = attackState;
            break;
        }
        sf::Int8 delim;
        if (!(l_packet >> delim) || (delim != (sf::Int8)Network::PlayerUpdateDelim)) {
            std::cout << "Faulty update!" << std::endl;
            break;
        }
    }
}