#include "S_Movement.h"
#include "SystemManager.h"
#include "Map.h"
#include <cmath>

S_Movement::S_Movement(SystemManager* l_sysMgr) : S_Base(System::Movement, l_sysMgr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::Position);
    req.TurnOnBit((unsigned int)Component::Movable);
    m_requiredComponents.push_back(req);
    req.Clear();
    m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Is_Moving, this);
    m_gameMap = nullptr;
}

S_Movement::~S_Movement() {}

void S_Movement::Update(float l_dT) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    for (auto& itr : m_entities) {
        C_Position* pos = entityManager->GetComponent<C_Position>(itr, Component::Position);
        C_Movable* mov = entityManager->GetComponent<C_Movable>(itr, Component::Movable);
        MovementStep(l_dT, mov, pos);
        pos->MoveBy(l_dT * mov->GetVelocity());
    }
}

void S_Movement::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    switch (l_event) {
    case EntityEvent::Colliding_X: StopEntity(l_entity, Axis::x); break;
    case EntityEvent::Colliding_Y: StopEntity(l_entity, Axis::y); break;
    case EntityEvent::Moving_Left: SetDirection(l_entity, Direction::Left); break;
    case EntityEvent::Moving_Right: SetDirection(l_entity, Direction::Right); break;
    case EntityEvent::Moving_Up: 
        C_Movable* mov = entityManager->GetComponent<C_Movable>(l_entity, Component::Movable);
        if (mov->GetVelocity().x == 0) SetDirection(l_entity, Direction::Up);
        break;
    case EntityEvent::Moving_Down:
        C_Movable* mov = entityManager->GetComponent<C_Movable>(l_entity, Component::Movable);
        if (mov->GetVelocity().x == 0) SetDirection(l_entity, Direction::Down);
        break;
    }
}

void S_Movement::Notify(const Message& l_msg) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    EntityMessage m = (EntityMessage)l_msg.m_type;
    switch (m) {
    case EntityMessage::Is_Moving:
        C_Movable* mov = entityManager->GetComponent<C_Movable>(l_msg.m_receiver, Component::Movable);
        if (mov->GetVelocity() != sf::Vector2f(0.f, 0.f)) break;
        m_systemManager->AddEvent(l_msg.m_receiver, (EventID)EntityEvent::Became_Idle);
        break;
    }
}

void S_Movement::StopEntity(const EntityId& l_entity, const Axis& l_axis) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    C_Movable* mov = entityManager->GetComponent<C_Movable>(l_entity, Component::Movable);
    if (l_axis == Axis::x) {
        mov->SetVelocity(sf::Vector2f(0, mov->GetVelocity().y));
    } else if (l_axis == Axis::y) {
        mov->SetVelocity(sf::Vector2f(mov->GetVelocity().x, 0));
    }
}

const sf::Vector2f& S_Movement::GetTileFriction(unsigned int l_elevation, unsigned int l_x, unsigned int l_y) {
    Tile* tile = nullptr;
    while (!tile && l_elevation >= 0) {
        tile = m_gameMap->GetTile(l_x, l_y, l_elevation);
        --l_elevation;
    }
    return (tile ? tile->m_properties->m_friction : m_gameMap->GetDefaultTile()->m_friction);
}

void S_Movement::SetMap(Map* l_gameMap) { m_gameMap = l_gameMap; }

void S_Movement::SetDirection(const EntityId& l_entity, const Direction& l_direction) {
    C_Movable* mov = m_systemManager->GetEntityManager()->GetComponent<C_Movable>(l_entity, Component::Movable);
    mov->SetDirection(l_direction);
    Message msg((MessageType)EntityMessage::Direction_Changed);
    msg.m_receiver = l_entity;
    msg.m_int = (int)l_direction;
    m_systemManager->GetMessageHandler()->Dispatch(msg);
}

void S_Movement::MovementStep(float l_dT, C_Movable* l_movable, C_Position* l_position) {
    sf::Vector2f f_coef = GetTileFriction(l_position->GetElevation(), l_position->GetPosition().x, l_position->GetPosition().y);
    sf::Vector2f friction(l_movable->GetSpeed().x * f_coef.x, l_movable->GetSpeed().y * f_coef.y);
    l_movable->AddVelocity(l_movable->GetAcceleration() * l_dT);
    l_movable->SetAcceleration(sf::Vector2f(0.f, 0.f));
    l_movable->ApplyFriction(friction);
    float magnitude = std::sqrt(l_movable->GetVelocity().x * l_movable->GetVelocity().x + 
        l_movable->GetVelocity().y * l_movable->GetVelocity().y);
    if (magnitude <= l_movable->GetMaxVelocity()) return;
    float maxv = l_movable->GetMaxVelocity();
    l_movable->SetVelocity(sf::Vector2f((l_movable->GetVelocity().x / magnitude) * maxv, 
        (l_movable->GetVelocity().y / magnitude) * maxv));
}