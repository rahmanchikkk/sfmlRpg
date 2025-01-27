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
    m_messages[(EntityMessage)l_msg.m_type].push_back(l_msg);
}

void S_Network::SetClient(Client* l_client) { m_client = l_client; }
void S_Network::SetPlayerId(const EntityId& l_entity) { m_playerId = l_entity; }
void S_Network::ClearSnapshots() { m_snapshots.clear(); }

void S_Network::AddSnapshot(const EntityId& l_entity, const sf::Int32& l_timestamp, const EntitySnapshot& l_snapshot) {
    sf::Lock lock(m_client->GetMutex());
    auto itr = m_snapshots.emplace(l_timestamp, SnapshotDetails());
    itr.first->second.m_snapshots.emplace(l_entity, l_snapshot);
}

void S_Network::ApplySnapshot(const EntityId& l_entity, const EntitySnapshot& l_snapshot, bool l_physics) {
    if (!HasEntity(l_entity)) return;
    ClientEntityManager* entityManager = (ClientEntityManager*)m_systemManager->GetEntityManager();
    C_Position* pos = nullptr;
    C_Movable* mov = nullptr;
    S_Movement* movement = nullptr;
    S_State* state = nullptr;
    C_Health* health = nullptr;
    C_Name* name = nullptr;
    sf::Lock lock(m_client->GetMutex());
    if (pos = entityManager->GetComponent<C_Position>(l_entity, Component::Position)) {
        pos->SetPosition(l_snapshot.m_position);
        pos->SetElevation(l_snapshot.m_elevation);
    }
    if (l_physics) {
        if (mov = entityManager->GetComponent<C_Movable>(l_entity, Component::Movable)) {
            mov->SetVelocity(l_snapshot.m_velocity);
            mov->SetAcceleration(l_snapshot.m_acceleration);
        }
    }
    if (state = m_systemManager->GetSystem<S_State>(System::State)) {
        state->ChangeState(l_entity, (EntityState)l_snapshot.m_state, false);
    }
    if (health = entityManager->GetComponent<C_Health>(l_entity, Component::Health)) {
        health->SetHealth(l_snapshot.m_health);
    }
    if (name = entityManager->GetComponent<C_Name>(l_entity, Component::Name)) {
        name->SetName(l_snapshot.m_name);
    }
    if (movement = m_systemManager->GetSystem<S_Movement>(System::Movement)) {
        movement->SetDirection(l_entity, (Direction)l_snapshot.m_direction);
    }
}

void S_Network::SendPlayerOutgoing() {
    sf::Int32 p_x, p_y;
    sf::Int8 p_attacked;
    for (auto& itr : m_messages) {
        if (itr.first == EntityMessage::Move) {
            int x = 0, y = 0;
            for (auto& m : itr.second) {
                if (m.m_int == (int)Direction::Up) --y;
                else if (m.m_int == (int)Direction::Down) ++y;
                else if (m.m_int == (int)Direction::Left) --x;
                else if (m.m_int == (int)Direction::Right);
            }
            if (!x && !y) continue;
            p_x = x; p_y = y;
        } else if (itr.first == EntityMessage::Attack) {
            p_attacked = true;
        }
    }
    sf::Packet packet;
    StampPacket(PacketType::PlayerUpdate, packet);
    packet << sf::Int8(EntityMessage::Move) << p_x << p_y << sf::Int8(Network::PlayerUpdateDelim);
    packet << sf::Int8(EntityMessage::Attack) << p_attacked << sf::Int8(Network::PlayerUpdateDelim);
    m_client->Send(packet);
    m_messages.clear();
}

void S_Network::PerformInterpolation() {
    if (m_snapshots.empty()) return;
    ClientEntityManager* entityManager = (ClientEntityManager*)m_systemManager->GetEntityManager();
    sf::Time t = m_client->GetTime();
    auto itr = ++m_snapshots.begin();
    while (itr != m_snapshots.end()) {
        if (m_snapshots.begin()->first <= t.asMilliseconds() - NET_RENDER_DELAY && 
            itr->first >= t.asMilliseconds() - NET_RENDER_DELAY) {
            auto snapshot1 = m_snapshots.begin();
            auto snapshot2 = itr;
            bool sortDrawables = false;
            for (auto snap = snapshot1->second.m_snapshots.begin(); snap != snapshot1->second.m_snapshots.end();) {
                if (!entityManager->HasEntity(snap->first)) {
                    if (entityManager->AddEntity(snap->second.m_type, snap->first) == (EntityId)Network::NullID) {
                        std::cout << "Failed to add entity from type: " << snap->second.m_type << std::endl;
                        continue;
                    }
                    ApplySnapshot(snap->first, snap->second, true);
                    ++snap;
                    continue;
                }
                auto snap2 = itr->second.m_snapshots.find(snap->first);
                if (snap2 == itr->second.m_snapshots.end()) {
                    sf::Lock lock(m_client->GetMutex());
                    entityManager->RemoveEntity(snap->first);
                    snap = snapshot1->second.m_snapshots.erase(snap);
                    continue;
                }
                EntitySnapshot iSnapshot;
                InterpolateSnapshot(snap->second, snapshot1->first, snap2->second, snapshot2->first, 
                    iSnapshot, t.asMilliseconds() - NET_RENDER_DELAY);
                ApplySnapshot(snap->first, iSnapshot, true);
                if (!CompareSnapshots(snap->second, snap2->second, true, false, false)) {
                    sortDrawables = true;
                }
                ++snap;
            }
            if (sortDrawables) m_systemManager->GetSystem<S_Renderer>(System::Renderer)->SortDrawables();
            return;
        }
        m_snapshots.erase(m_snapshots.begin());
        itr = ++m_snapshots.begin();
    }
}