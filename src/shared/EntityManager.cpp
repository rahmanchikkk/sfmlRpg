#include "EntityManager.h"
#include "SystemManager.h"

EntityManager::EntityManager(SystemManager* l_sysMgr) : m_systemManager(l_sysMgr), m_idCounter(0) {}

EntityManager::~EntityManager() { Purge(); }

int EntityManager::AddEntity(const Bitmask& l_bits, int l_id) {
    unsigned int eid = (l_id == -1 ? m_idCounter : l_id);
    if (!m_entities.emplace(eid, EntityData()).second) return -1;
    if (l_id == -1) ++m_idCounter;
    for (int i = 0; i < N_COMPONENT_TYPES; ++i) {
        if (l_bits.GetBit(i)) {
            AddComponent(eid, (Component)i);
        }
    }
    m_systemManager->EntityModified(eid, l_bits);
    m_systemManager->AddEvent(eid, EntityEvent::Spawned);
    return eid;
}

int EntityManager::AddEntity(const std::string& l_entityFile, int l_id) {
    int entityId = -1;
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + "media/Entities/" + l_entityFile + ".entity");
    if (!file.is_open()) {
        std::cout << "Failed to load entity from file: " << l_entityFile << std::endl;
        return -1;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') continue;
        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "Name") {

        } else if (type == "Attributes") {
            if (entityId != -1) continue;
            Bitmask mask;
            Bitset set;
            keystream >> set;
            mask.SetMask(set);
            entityId = AddEntity(mask);
            if (entityId == -1) return -1;
        } else if (type == "Component") {
            if (entityId == -1) continue;
            unsigned int cid;
            keystream >> cid;
            C_Base* c = GetComponent<C_Base>(entityId, (Component)cid);
            if (!c) continue;
            keystream >> *c;
        }
    }
    file.close();
    m_entities.at(entityId).m_type = l_entityFile;
    return entityId;
}

void EntityManager::SetSystemManager(SystemManager* l_sysMgr) { m_systemManager = l_sysMgr; }

bool EntityManager::RemoveEntity(const EntityId& l_entity) {
    auto itr = m_entities.find(l_entity);
    if (itr == m_entities.end()) return false;
    Message msg((MessageType)EntityMessage::Removed_Entity);
    msg.m_receiver = l_entity;
    msg.m_int = l_entity;
    m_systemManager->GetMessageHandler()->Dispatch(msg);
    while (itr->second.m_components.begin() != itr->second.m_components.end()) {
        delete *itr->second.m_components.begin();
        itr->second.m_components.erase(itr->second.m_components.begin());
    }
    m_entities.erase(itr);
    m_systemManager->RemoveEntity(l_entity);
    return true;
}

bool EntityManager::HasEntity(const EntityId& l_entity) {
    auto itr = m_entities.find(l_entity);
    return (itr != m_entities.end());
}

bool EntityManager::AddComponent(const EntityId& l_entity, const Component& l_component) {
    auto itr = m_entities.find(l_entity);
    if (itr == m_entities.end()) return false;
    if (itr->second.m_mask.GetBit((unsigned int)l_component)) return false;
    auto c = m_compFactory.find(l_component);
    if (c == m_compFactory.end()) return false;
    C_Base* component = c->second();
    itr->second.m_components.emplace_back(c);
    itr->second.m_mask.TurnOnBit((unsigned int)l_component);
    m_systemManager->EntityModified(l_entity, itr->second.m_mask);
    return true;
}

bool EntityManager::RemoveComponent(const EntityId& l_entity, const Component& l_component) {
    auto itr = m_entities.find(l_entity);
    if (itr == m_entities.end()) return false;
    if (!itr->second.m_mask.GetBit((unsigned int)l_component)) return false;
    auto& container = itr->second.m_components;
    auto c = std::find_if(container.begin(), container.end(), [&l_component](C_Base* b) {
        return b->GetType() == l_component;
    });
    if (c == container.end()) return false;
    delete (*c);
    container.erase(c);
    itr->second.m_mask.ClearBit((unsigned int)l_component);
    m_systemManager->EntityModified(l_entity, itr->second.m_mask);
    return true;
}

bool EntityManager::HasComponent(const EntityId& l_entity, const Component& l_component) {
    auto entity = m_entities.find(l_entity);
    if (entity == m_entities.end()) return false;
    auto itr = std::find_if(entity->second.m_components.begin(), entity->second.m_components.end(), [&l_component](C_Base* b) {
        return b->GetType() == l_component;
    });
    return (itr != entity->second.m_components.end());
}

void EntityManager::Purge() {
    for (auto& itr : m_entities) {
        while (itr.second.m_components.begin() != itr.second.m_components.end()) {
            delete (*itr.second.m_components.begin());
            itr.second.m_components.erase(itr.second.m_components.begin());
        }
        itr.second.m_mask.Clear();
    }
    m_entities.clear();
    m_idCounter = 0;
}

unsigned int EntityManager::GetEntityCount() { return m_idCounter; }