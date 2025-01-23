#pragma once
#include "ECS_Types.h"
#include "Observer.h"
#include "EntityManager.h"
#include "Bitmask.h"
#include "EventQueue.h"
#include "EntityEvents.h"
#include <vector>
#include <algorithm>

class SystemManager;

using EntityList = std::vector<EntityId>;
using Requirements = std::vector<Bitmask>;

class S_Base : public Observer {
public:
    S_Base(const System& l_id, SystemManager* l_sysMgr);
    ~S_Base();

    bool AddEntity(const EntityId& l_entity);
    bool RemoveEntity(const EntityId& l_entity);
    bool HasEntity(const EntityId& l_entity);

    System GetId();
    bool FitsRequirements(const Bitmask& l_bits);

    void Purge();
    virtual void Update(float l_dT) = 0;
    virtual void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event) = 0;
protected:  
    System m_id;
    EntityList m_entities;
    Requirements m_requiredComponents;
    SystemManager* m_systemManager;
};