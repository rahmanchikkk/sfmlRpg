#include "ServerEntityManager.h"
#include "SystemManager.h"

ServerEntityManager::ServerEntityManager(SystemManager* l_sysMgr) : EntityManager(l_sysMgr) {
    AddComponentType<C_Position>(Component::Position);
    AddComponentType<C_Movable>(Component::Movable);
    AddComponentType<C_State>(Component::State);
    AddComponentType<C_Collidable>(Component::Collidable);
    AddComponentType<C_Controller>(Component::Controller);
    AddComponentType<C_Client>(Component::Client);
    AddComponentType<C_Name>(Component::Name);
    AddComponentType<C_Health>(Component::Health);
    AddComponentType<C_Attacker>(Component::Attacker);
}

ServerEntityManager::~ServerEntityManager() {}

void ServerEntityManager::DumpEntityInfo(sf::Packet& l_packet) {
    for (auto& entity : m_entities) {
        l_packet << sf::Int32(entity.first);
        EntitySnapshot snapshot;
        snapshot.m_type = entity.second.m_type;
        if (entity.second.m_mask.GetBit((unsigned int)Component::Position)) {
            C_Position* pos = GetComponent<C_Position>(entity.first, Component::Position);
            snapshot.m_position = pos->GetPosition();
            snapshot.m_elevation = pos->GetElevation();
        }
        if (entity.second.m_mask.GetBit((unsigned int)Component::Movable)) {
            C_Movable* mov = GetComponent<C_Movable>(entity.first, Component::Movable);
            snapshot.m_velocity = mov->GetVelocity();
            snapshot.m_acceleration = mov->GetAcceleration();
            snapshot.m_direction = (sf::Uint8)mov->GetDirection();
        }
        if (entity.second.m_mask.GetBit((unsigned int)Component::State)) {
            C_State* state = GetComponent<C_State>(entity.first, Component::State);
            snapshot.m_state = (sf::Uint8)state->GetState();
        }
        if (entity.second.m_mask.GetBit((unsigned int)Component::Name)) {
            C_Name* name = GetComponent<C_Name>(entity.first, Component::Name);
            snapshot.m_name = name->GetName();
        }
        if (entity.second.m_mask.GetBit((unsigned int)Component::Health)) {
            C_Health* health = GetComponent<C_Health>(entity.first, Component::Health);
            snapshot.m_health = sf::Uint8(health->GetHealth());
        }
        l_packet << snapshot;
    }
}