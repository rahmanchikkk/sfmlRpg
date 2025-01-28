#include "ServerSystemManager.h"

ServerSystemManager::ServerSystemManager() {
    AddSystem<S_State>(System::State);
    AddSystem<S_Control>(System::Control);
    AddSystem<S_Movement>(System::Movement);
    AddSystem<S_Collision>(System::Collision);
    AddSystem<S_Combat>(System::Combat);
    AddSystem<S_Network>(System::Network);
    AddSystem<S_Timers>(System::Timers);
}

ServerSystemManager::~ServerSystemManager() {}