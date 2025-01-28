#include "S_Combat.h"
#include "SystemManager.h"

S_Combat::S_Combat(SystemManager* l_sysMgr) : S_Base(System::Combat, l_sysMgr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::Position);
    req.TurnOnBit((unsigned int)Component::Movable);
    req.TurnOnBit((unsigned int)Component::State);
    req.TurnOnBit((unsigned int)Component::Health);
    m_requiredComponents.push_back(req);
    req.ClearBit((unsigned int)Component::Health);
    req.TurnOnBit((unsigned int)Component::Attacker);
    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Being_Attacked, this);
}

S_Combat::~S_Combat() {}

void S_Combat::Update(float l_dT) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    for (auto& itr : m_entities) {
        C_Attacker* attacker = entityManager->GetComponent<C_Attacker>(itr, Component::Attacker);
        if (!attacker) continue;
        sf::Vector2f offset = attacker->GetOffset();
        sf::FloatRect aoa = attacker->GetAreaOfAttack();
        Direction direction = entityManager->GetComponent<C_Movable>(itr, Component::Movable)->GetDirection();
        sf::Vector2f position = entityManager->GetComponent<C_Position>(itr, Component::Position)->GetPosition();
        if (direction == Direction::Up) offset.y -= (aoa.height / 2);
        else if (direction == Direction::Down) offset.y += (aoa.height / 2);
        else if (direction == Direction::Left) offset.x -= (aoa.width / 2);
        else if (direction == Direction::Right) offset.x += (aoa.width / 2);
        position -= sf::Vector2f(aoa.width / 2, aoa.height / 2);
        attacker->SetAreaPosition(position + offset);
    }
}

void S_Combat::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event) {}

void S_Combat::Notify(const Message& l_msg) {
    if (!HasEntity(l_msg.m_receiver) || !HasEntity(l_msg.m_sender)) return;
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    EntityMessage m = (EntityMessage)l_msg.m_type;
    switch (m) {
    case EntityMessage::Being_Attacked:
        C_Attacker* attacker = entityManager->GetComponent<C_Attacker>(l_msg.m_sender, Component::Attacker);
        C_Health* victim = entityManager->GetComponent<C_Health>(l_msg.m_receiver, Component::Health);
        if (!attacker || !victim) return;
        S_State* state = m_systemManager->GetSystem<S_State>(System::State);
        if (state->GetState(l_msg.m_sender) != EntityState::Attacking) return;
        if (!attacker->HasAttacked()) return;
        victim->SetHealth((victim->GetHealth() > 0 ? victim->GetHealth() - 1 : 0));
        if (!victim->GetHealth()) {
            state->ChangeState(l_msg.m_receiver, EntityState::Dying, true);
        } else {
            Message msg((MessageType)EntityMessage::Hurt);
            msg.m_receiver = l_msg.m_receiver;
            m_systemManager->GetMessageHandler()->Dispatch(msg);
        }
        Direction attackerDirection = entityManager->GetComponent<C_Movable>(l_msg.m_sender, Component::Movable)->GetDirection();
        float knockback = attacker->GetKnockback();
        sf::Vector2f knockbackVelocity;
        if (attackerDirection == Direction::Left || attackerDirection == Direction::Up) knockback = -knockback;
        if (attackerDirection == Direction::Left || attackerDirection == Direction::Right) knockbackVelocity.x = knockback;
        else knockbackVelocity.y = knockback;
        entityManager->GetComponent<C_Movable>(l_msg.m_receiver, Component::Movable)->SetVelocity(knockbackVelocity);
        break;
    }
}