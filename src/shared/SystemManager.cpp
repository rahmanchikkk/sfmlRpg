#include "SystemManager.h"
#include "EntityManager.h"

SystemManager::SystemManager() { m_entityManager = nullptr; }

SystemManager::~SystemManager() { PurgeSystems(); }

EntityManager* SystemManager::GetEntityManager() { return m_entityManager; }

MessageHandler* SystemManager::GetMessageHandler() { return &m_messages; }

void SystemManager::SetEntityManager(EntityManager* l_entityMgr) {
    if (!m_entityManager) {
        m_entityManager = l_entityMgr; 
    }
}

void SystemManager::AddEvent(const EntityId& l_entity, const EventID& l_event) {
    auto itr = m_events.find(l_entity);
    if (itr == m_events.end()) return;
    itr->second.AddEvent(l_event);
}

void SystemManager::Update(float l_dT) {
    for (auto& itr : m_systems) {
        itr.second->Update(l_dT);
    }
    HandleEvents();
}

void SystemManager::HandleEvents() {
    for (auto& itr : m_events) {
        EventID id = 0;
        while (itr.second.ProcessEvents(id)) {
            for (auto& sys : m_systems) {
                if (sys.second->HasEntity(itr.first)) {
                    sys.second->HandleEvent(itr.first, (EntityEvent)id);
                }
            }
        }
    }
}

void SystemManager::EntityModified(const EntityId& l_entity, const Bitmask& l_bits) {
    for (auto& itr : m_systems) {
        if (itr.second->FitsRequirements(l_bits)) {
            if (!itr.second->HasEntity(l_entity)) {
                itr.second->AddEntity(l_entity);
            }
        } else {
            if (itr.second->HasEntity(l_entity)) {
                itr.second->RemoveEntity(l_entity);
            }
        }
    }
}

void SystemManager::RemoveEntity(const EntityId& l_entity) {
    for (auto& itr : m_systems) {
        if (itr.second->HasEntity(l_entity)) {
            itr.second->RemoveEntity(l_entity);
        }
    }
}

void SystemManager::PurgeSystems() {
    for (auto& itr : m_systems) {
        delete itr.second;
    }
    m_systems.clear();
}

void SystemManager::PurgeEntities() {
    for (auto& itr : m_systems) {
        itr.second->Purge();
    }
}