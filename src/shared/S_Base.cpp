#include "S_Base.h"
#include "SystemManager.h"

S_Base::S_Base(const System& l_id, SystemManager* l_sysMgr) : m_id(l_id), m_systemManager(l_sysMgr) {}

S_Base::~S_Base() { Purge(); }

bool S_Base::AddEntity(const EntityId& l_entity) {
    if (HasEntity(l_entity)) return false;
    m_entities.emplace_back(l_entity);
    return true;
}

bool S_Base::RemoveEntity(const EntityId& l_entity) {
    auto itr = std::find_if(m_entities.begin(), m_entities.end(), [&l_entity](const EntityId& e) {
        return e == l_entity;
    });
    if (itr == m_entities.end()) return false;
    m_entities.erase(itr);
    return true;
}

bool S_Base::HasEntity(const EntityId& l_entity) {
    auto itr = std::find_if(m_entities.begin(), m_entities.end(), [&l_entity](const EntityId& e) {
        return e == l_entity;
    });
    return (itr != m_entities.end());
}

System S_Base::GetId() { return m_id; };

bool S_Base::FitsRequirements(const Bitmask& l_bits) {
    auto itr = std::find_if(m_requiredComponents.begin(), m_requiredComponents.end(), [&l_bits](Bitmask b) {
        return b.Matches(l_bits, b.GetMask());
    });
    return (itr != m_requiredComponents.end());
}

void S_Base::Purge() { m_entities.clear(); }