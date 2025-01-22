#pragma once
#include "ECS_Types.h"
#include "Observer.h"
#include "EntityManager.h"
#include "Bitmask.h"
#include "EventQueue.h"
#include "EntityEvents.h"
#include <vector>

class SystemManager;

using EntityList = std::vector<EntityId>;
using Requirements = std::vector<Bitmask>;

class S_Base : public Observer {
public:
    S_Base(const System& l_id, SystemManager* l_sysMgr);
    ~S_Base();
protected:  
    System m_id;
    EntityList m_entities;
    Requirements m_requiredComponents;
    SystemManager* m_systemManager;
};