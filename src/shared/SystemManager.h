#pragma once
#include <SFML/Graphics.hpp>
#include "S_Base.h"
#include <unordered_map>
#include "EventQueue.h"
#include "MessageHandler.h"

class EntityManager;

using SystemContainer = std::unordered_map<System, S_Base*>;
using EntityEventContainer = std::unordered_map<EntityId, EventQueue>;

class SystemManager {
public:
    SystemManager(EntityManager* l_entityMgr);
    virtual ~SystemManager();

    EntityManager* GetEntityManager();
    MessageHandler* GetMessageHandler();
    void SetEntityManager(EntityManager* l_entityMgr);

    template <class T>
    bool AddSystem(const System& l_type) {
        auto itr = m_systems.find(l_type);
        if (itr != m_systems.end()) return false;
        m_systems.at(l_type) = new T(this);
        return true;
    };

    template <class T>
    T* GetSystem(const System& l_type) {
        auto itr = m_systems.find(l_type);
        return (itr == m_systems.end() ? nullptr : dynamic_cast<T*>(itr->second));
    };

    void AddEvent(const EntityId& l_entity, const EntityEvent& l_event);
    void Update(float l_dT);
    void HandleEvents();

    void EntityModified(const EntityId& l_entity, const Bitmask& l_bits);
    void RemoveEntity(const EntityId& l_entity);

    void PurgeSystems();
    void PurgeEntities();
protected:
    EntityManager* m_entityManager;
    MessageHandler m_messages;
    SystemContainer m_systems;
    EntityEventContainer m_events;
};