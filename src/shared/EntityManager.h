#pragma once
#include "C_Base.h"
#include "Bitmask.h"
#include "Utilities.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include <fstream>

using EntityId = int;
using ComponentContainer = std::vector<C_Base*>;

struct EntityData {
    Bitmask m_mask;
    std::string m_type;
    ComponentContainer m_components;
};

using EntityContainer = std::unordered_map<EntityId, EntityData>;
using ComponentFactory = std::unordered_map<Component, std::function<C_Base*(void)>>;

class SystemManager;

class EntityManager {
public:
    EntityManager(SystemManager* l_sysMgr);
    virtual ~EntityManager();

    void SetSystemManager(SystemManager* l_sysMgr);
    int AddEntity(const Bitmask& l_bits, int l_id = -1);
    virtual int AddEntity(const std::string& l_entityFile, int l_id = -1);

    bool RemoveEntity(const EntityId& l_entity);
    bool HasEntity(const EntityId& l_entity);

    template <class T>
    T* GetComponent(const EntityId& l_entity, const Component& l_type) {
        auto itr = m_entities.find(l_entity);
        if (itr == m_entities.end()) return false;
        if (!itr->second.m_mask.GetBit((unsigned int)l_type)) return false;
        auto container = itr->second.m_components;
        auto component = std::find_if(container.begin(), container.end(), [&l_type](C_Base* b) {
            return b->GetType() == l_type;
        });
        return (component == container.end() ? nullptr : dynamic_cast<T*>(*component));
    };

    bool AddComponent(const EntityId& l_entity, const Component& l_component);
    bool RemoveComponent(const EntityId& l_entity, const Component& l_component);
    bool HasComponent(const EntityId& l_entity, const Component& l_component);

    void Purge();
    unsigned int GetEntityCount();
protected:
    template <class T>
    void AddComponentType(const Component& l_type) {
        m_compFactory[l_type] = []()->C_Base*{
            return new T();
        };
    };

    SystemManager* m_systemManager;
    EntityContainer m_entities;
    ComponentFactory m_compFactory;
    unsigned int m_idCounter;
};