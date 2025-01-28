#pragma once
#include "SystemManager.h"
#include "S_Network.h"
#include "S_Movement.h"
#include "S_Collision.h"
#include "S_Timers.h"
#include "S_State.h"
#include "S_Control.h"
#include "S_Combat.h"

class ServerSystemManager : public SystemManager {
public:
    ServerSystemManager();
    ~ServerSystemManager();
private:

};