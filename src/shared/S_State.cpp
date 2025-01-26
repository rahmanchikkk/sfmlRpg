#include "S_State.h"
#include "SystemManager.h"

S_State::S_State(SystemManager* l_sysMgr) : S_Base(System::State, l_sysMgr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::State);
    m_requiredComponents.emplace_back(req);
    req.Clear();
    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Switch_State, this);
    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Hurt, this);
    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Attack, this);
    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Move, this);
}

S_State::~S_State() {}

void S_State::Update(float l_dT) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    for (auto& itr : m_entities) {
        C_State* state = entityManager->GetComponent<C_State>(itr, Component::State);
        if (state->GetState() != EntityState::Walking) continue;
        Message msg((MessageType)EntityMessage::Is_Moving);
        msg.m_receiver = itr;
        m_systemManager->GetMessageHandler()->Dispatch(msg);
    }
}

void S_State::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event) {
    switch (l_event) {
    case EntityEvent::Became_Idle:
        ChangeState(l_entity, EntityState::Idle, false);
        break;
    }
}

void S_State::Notify(const Message& l_msg) {
    if (!HasEntity(l_msg.m_receiver)) return;
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    EntityMessage m = (EntityMessage)l_msg.m_type;
    switch (m) {
    case EntityMessage::Move:
        C_State* state = entityManager->GetComponent<C_State>(l_msg.m_receiver, Component::State);
        if (state->GetState() == EntityState::Dying || state->GetState() == EntityState::Attacking) break;
        EntityEvent event;
        if ((Direction)l_msg.m_int == Direction::Up) {
            event = EntityEvent::Moving_Up;
        } else if ((Direction)l_msg.m_int == Direction::Down) {
            event = EntityEvent::Moving_Down;
        } else if ((Direction)l_msg.m_int == Direction::Left) {
            event = EntityEvent::Moving_Left;
        } else if ((Direction)l_msg.m_int == Direction::Right) {
            event = EntityEvent::Moving_Right;
        }
        m_systemManager->AddEvent(l_msg.m_receiver, (EventID)event);
        ChangeState(l_msg.m_receiver, EntityState::Walking, false);
        break;
    case EntityMessage::Attack:
        C_State* state = entityManager->GetComponent<C_State>(l_msg.m_receiver, Component::State);
        if (state->GetState() != EntityState::Hurt && state->GetState() != EntityState::Dying) {
            m_systemManager->AddEvent(l_msg.m_receiver, (EventID)EntityEvent::Began_Attacking);
            ChangeState(l_msg.m_receiver, EntityState::Attacking, false);
        }
        break;
    case EntityMessage::Hurt:
        C_State* state = entityManager->GetComponent<C_State>(l_msg.m_receiver, Component::State);
        if (state->GetState() != EntityState::Dying) {
            ChangeState(l_msg.m_receiver, EntityState::Hurt, false);
        }
        break;
    case EntityMessage::Switch_State:
        ChangeState(l_msg.m_receiver, (EntityState)l_msg.m_int, false);
        break;
    }
}

void S_State::ChangeState(const EntityId& l_entity, const EntityState& l_state, bool l_force) {
    C_State* state = m_systemManager->GetEntityManager()->GetComponent<C_State>(l_entity, Component::State);
    if (state->GetState() == l_state) return;
    if (!l_force && l_state == EntityState::Dying) return;
    state->SetState(l_state);
    Message msg((MessageType)EntityMessage::State_Changed);
    msg.m_receiver = l_entity;
    msg.m_int = (int)l_state;
    m_systemManager->GetMessageHandler()->Dispatch(msg);
}

EntityState S_State::GetState(const EntityId& l_entity) {
    C_State* state = m_systemManager->GetEntityManager()->GetComponent<C_State>(l_entity, Component::State);
    return state->GetState();
}