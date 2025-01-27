#pragma once
#include "S_Base.h"
#include "EntitySnapshot.h"
#include "ClientEntityManager.h"
#include "Client.h"
#include "S_State.h"
#include "S_Movement.h"
#include "EntityMessage.h"
#include <map>
#include <unordered_map>
#include "NetSettings.h"

template <class T>
inline T Interpolate(const sf::Int32& T1, const sf::Int32& T2, const T& T1_VAL, const T& T2_VAL, const sf::Int32& TX) {
    return ((T2_VAL - T1_VAL) / (T2 - T1) * (TX - T1)) + T1_VAL;
};

void InterpolateSnapshot(const EntitySnapshot& l_s1, const sf::Int32& l_t1, const EntitySnapshot& l_s2, const sf::Int32& l_t2, EntitySnapshot& l_target, const sf::Int32& l_tx);

bool CompareSnapshots(const EntitySnapshot& l_s1, const EntitySnapshot& l_s2, bool l_position = false, bool l_physics = false, bool l_state = false);

using SnapshotMap = std::unordered_map<EntityId, EntitySnapshot>;

struct SnapshotDetails {
    SnapshotMap m_snapshots;
};

using SnapshotContainer = std::map<sf::Int32, SnapshotDetails>;
using OutgoingMessages = std::unordered_map<EntityMessage, std::vector<Message>>;

class S_Network : public S_Base {
public:
    S_Network(SystemManager* l_sysMgr);
    ~S_Network();

    void Update(float l_dT);
    void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
    void Notify(const Message& l_msg);

    void SetClient(Client* l_client);
    void SetPlayerId(const EntityId& l_entity);

    void AddSnapshot(const EntityId& l_entity, const sf::Int32& l_timestamp, const EntitySnapshot& l_snapshot);
    void SendPlayerOutgoing();
    void ClearSnapshots();
private:
    void ApplySnapshot(const EntityId& l_entity, const EntitySnapshot& l_snapshot, bool l_physics);
    void PerformInterpolation();
    
    Client* m_client;
    SnapshotContainer m_snapshots;
    OutgoingMessages m_messages;
    EntityId m_playerId;
    sf::Time m_playerUpdateTimer;
};