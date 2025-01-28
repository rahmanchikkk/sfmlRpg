#include "S_Timers.h"
#include "SystemManager.h"

S_Timers::S_Timers(SystemManager* l_sysMgr) : S_Base(System::Timers, l_sysMgr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::State);
    req.TurnOnBit((unsigned int)Component::Health);
    m_requiredComponents.push_back(req);
    req.ClearBit((unsigned int)Component::Health);
    req.TurnOnBit((unsigned int)Component::Attacker);
    m_requiredComponents.push_back(req);
    req.Clear();
}

S_Timers::~S_Timers() {}

void S_Timers::Update(float l_dT) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    for (auto& itr : m_entities) {
        EntityState state = entityManager->GetComponent<C_State>(itr, Component::State)->GetState();
        if (state == EntityState::Attacking) {
            C_Attacker* attacker = entityManager->GetComponent<C_Attacker>(itr, Component::Attacker);
            attacker->AddToTimer(sf::milliseconds(l_dT));
            if (attacker->GetTimer().asMilliseconds() < attacker->GetAttackDuration()) continue;
            attacker->Reset();
            attacker->SetAttacked(false);
        } else if (state == EntityState::Hurt || state == EntityState::Dying) {
            C_Health* health = entityManager->GetComponent<C_Health>(itr, Component::Health);
            health->AddToTimer(sf::milliseconds(l_dT));
            if ((state == EntityState::Hurt && health->GetTimer().asMilliseconds() < health->GetHurtDuration()) ||
                (state == EntityState::Dying && health->GetTimer().asMilliseconds() < health->GetDeathDuration())) {
                continue;
            }
            health->Reset();
            if (state == EntityState::Dying) {
                Message msg((MessageType)EntityMessage::Respawn);
                msg.m_receiver = itr;
                m_systemManager->GetMessageHandler()->Dispatch(msg);
                health->ResetHealth();
            }
        } else continue;
        S_State* stateSystem = m_systemManager->GetSystem<S_State>(System::State);
        stateSystem->ChangeState(itr, EntityState::Idle, true);
    }
}

void S_Timers::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event) {}
void S_Timers::Notify(const Message& l_msg) {}