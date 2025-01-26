#pragma once
#include "S_Base.h"
#include "C_Position.h"
#include "C_Movable.h"
#include "C_Controller.h"

class S_Control : public S_Base {
public:
    S_Control(SystemManager* l_sysMgr);
    ~S_Control();

    void Update(float l_dT);
    void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
    void Notify(const Message& l_msg);

    void MoveEntity(const EntityId& l_entity, const Direction& l_direction);
private:

};