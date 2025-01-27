#include "S_Network.h"
#include "SystemManager.h"
#include "S_Renderer.h"

void InterpolateSnapshot(const EntitySnapshot& l_s1, const sf::Int32& l_t1, const EntitySnapshot& l_s2, const sf::Int32& l_t2, EntitySnapshot& l_target, const sf::Int32& l_tx) {
    l_target.m_direction = l_s1.m_direction;
    l_target.m_state = l_s1.m_state;
    l_target.m_name = l_s1.m_name;
    l_target.m_health = l_s1.m_health;
    l_target.m_type = l_s1.m_type;

    l_target.m_position.x = Interpolate<float>(l_t1, l_t2, l_s1.m_position.x, l_s2.m_position.y, l_tx);
    l_target.m_position.x = Interpolate<float>(l_t1, l_t2, l_s1.m_position.x, l_s2.m_position.y, l_tx);

    l_target.m_velocity.x = Interpolate<float>(l_t1, l_t2, l_s1.m_velocity.x, l_s2.m_velocity.y, l_tx);
    l_target.m_velocity.x = Interpolate<float>(l_t1, l_t2, l_s1.m_velocity.x, l_s2.m_velocity.y, l_tx);
    
    l_target.m_acceleration.x = Interpolate<float>(l_t1, l_t2, l_s1.m_acceleration.x, l_s2.m_acceleration.y, l_tx);
    l_target.m_acceleration.x = Interpolate<float>(l_t1, l_t2, l_s1.m_acceleration.x, l_s2.m_acceleration.y, l_tx);
}

bool CompareSnapshots(const EntitySnapshot& l_s1, const EntitySnapshot& l_s2, bool l_position, bool l_physics, bool l_state) {
    if (l_position && (l_s1.m_position != l_s2.m_position)) return false;
    if (l_physics && (l_s1.m_velocity != l_s2.m_velocity || l_s1.m_acceleration != l_s2.m_acceleration)) return false;
    if (l_state && (l_s1.m_state != l_s2.m_state)) return false;
    return true;
}

S_Network::S_Network(SystemManager* l_sysMgr) : S_Base(System::Network, l_sysMgr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::Client);
    m_requiredComponents.push_back(req);
    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Move, this);
    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Attack, this);
    m_playerUpdateTimer = sf::milliseconds(0);
}

S_Network::~S_Network() {}

void S_Network::Update(float l_dT) {
    if (!m_client) return;
    sf::Lock lock(m_client->GetMutex());
    m_playerUpdateTimer += sf::milliseconds(l_dT);
    if (m_playerUpdateTimer.asMilliseconds() >= PLAYER_UPDATE_INTERVAL) {
        SendPlayerOutgoing();
        m_playerUpdateTimer = sf::milliseconds(0);
    }
    PerformInterpolation();
}

void S_Network::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event) {}

void S_Network::Notify(const Message& l_msg) {
    if (!HasEntity(l_msg.m_receiver) || l_msg.m_receiver != m_playerId) return;
    if (l_msg.m_int == (int)EntityMessage::Attack && m_messages.find(EntityMessage::Attack) == m_messages.end()) return;
    m_messages[EntityMessage::Attack].push_back(l_msg);
}

void S_Network::SetClient(Client* l_client) { m_client = l_client; }
void S_Network::SetPlayerId(const EntityId& l_entity) { m_playerId = l_entity; }
void S_Network::ClearSnapshots() {
    sf::Lock lock(m_client->GetMutex());
    m_snapshots.clear();
}

void S_Network::AddSnapshot(const EntityId& l_entity, const sf::Int32& l_timestamp, const EntitySnapshot& l_snapshot) {
    sf::Lock lock(m_client->GetMutex());
    auto itr = m_snapshots.emplace(l_timestamp, SnapshotDetails());
    itr.first->second.m_snapshots.emplace(l_entity, l_snapshot);
}

void S_Network::ApplyEntitySnapshot(const EntityId& l_entity, const EntitySnapshot& l_snapshot, bool l_physics) {

}