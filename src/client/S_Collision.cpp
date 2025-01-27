#include "S_Collision.h"
#include "SystemManager.h"
#include "Map.h"
#include <cmath>

S_Collision::S_Collision(SystemManager* l_sysMgr) : S_Base(System::Collision, l_sysMgr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::Position);
    req.TurnOnBit((unsigned int)Component::Collidable);
    m_requiredComponents.push_back(req);
    req.Clear();
    m_gameMap = nullptr;
}

S_Collision::~S_Collision() {}

void S_Collision::Update(float l_dT) {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    for (auto& itr : m_entities) {
        C_Position* pos = entityManager->GetComponent<C_Position>(itr, Component::Position);
        C_Collidable* col = entityManager->GetComponent<C_Collidable>(itr, Component::Collidable);
        col->SetPosition(pos->GetPosition());
        col->ResetCollisionFlags();
        CheckOutOfBounds(pos, col);
        MapCollisions(itr, pos, col);
    }
    EntityCollisions();
}

void S_Collision::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event) {}
void S_Collision::Notify(const Message& l_msg) {}

void S_Collision::EntityCollisions() {
    EntityManager* entityManager = m_systemManager->GetEntityManager();
    for (auto itr = m_entities.begin(); itr != m_entities.end(); itr = std::next(itr)) {
        for (auto itr2 = std::next(itr); itr2 != m_entities.end(); itr2 = std::next(itr2)) {
            C_Collidable* c1 = entityManager->GetComponent<C_Collidable>(*itr, Component::Collidable);
            C_Collidable* c2 = entityManager->GetComponent<C_Collidable>(*itr2, Component::Collidable);
            if (c1->GetCollidable().intersects(c2->GetCollidable())) {
                //Entity-on-entity collisions!
            }
        }
    }
}

void S_Collision::CheckOutOfBounds(C_Position* l_position, C_Collidable* l_collidable) {
    unsigned int tileSize = Sheet::Tile_Size;
    if (l_position->GetPosition().x < 0) {
        l_position->SetPosition(0, l_position->GetPosition().y);
        l_collidable->SetPosition(l_position->GetPosition());
    } else if (l_position->GetPosition().x > m_gameMap->GetMapSize().x * tileSize) {
        l_position->SetPosition(m_gameMap->GetMapSize().x * tileSize, l_position->GetPosition().y);
        l_collidable->SetPosition(l_position->GetPosition());
    }
    if (l_position->GetPosition().y < 0) {
        l_position->SetPosition(l_position->GetPosition().x, 0);
        l_collidable->SetPosition(l_position->GetPosition());
    } else if (l_position->GetPosition().y > m_gameMap->GetMapSize().y * tileSize) {
        l_position->SetPosition(l_position->GetPosition().x, m_gameMap->GetMapSize().y * tileSize);
        l_collidable->SetPosition(l_position->GetPosition());
    }
}

void S_Collision::MapCollisions(const EntityId& l_entity, C_Position* l_position, C_Collidable* l_collidable) {
    unsigned int tileSize = Sheet::Tile_Size;
    Collisions collisions;
    sf::FloatRect entityAABB = l_collidable->GetCollidable();
    int fromX = std::floor(l_collidable->GetCollidable().left / tileSize);
    int toX = std::floor((l_collidable->GetCollidable().left + l_collidable->GetCollidable().width) / tileSize);
    int fromY = std::floor(l_collidable->GetCollidable().top / tileSize);
    int toY = std::floor((l_collidable->GetCollidable().top + l_collidable->GetCollidable().height) / tileSize);
    for (int x = fromX; x <= toX; ++x) {
        for (int y = fromY; y <= toY; ++y) {
            for (int l = 0; l < Sheet::Num_Layers; ++l) {
                Tile* tile = m_gameMap->GetTile(x, y, l);
                if (!tile) continue;
                if (!tile->m_solid) continue;
                sf::FloatRect tileAABB(x * tileSize, y * tileSize, tileSize, tileSize);
                sf::FloatRect intersection;
                entityAABB.intersects(tileAABB, intersection);
                float area = intersection.width * intersection.height;
                CollisionElement* c = new CollisionElement(area, tile->m_properties, tileAABB);
                collisions.emplace_back(c);
                break;
            }
        }
    }
    if (collisions.empty()) return;
    std::sort(collisions.begin(), collisions.end(), [](CollisionElement* a, CollisionElement* b) {
        return a->m_area > b->m_area;
    });
    for (auto& col : collisions) {
        entityAABB = l_collidable->GetCollidable();
        if (!entityAABB.intersects(col->m_tileBounds)) continue;
        float diffX = ((entityAABB.left + (entityAABB.width / 2)) - (col->m_tileBounds.left + (col->m_tileBounds.width / 2)));
        float diffY = ((entityAABB.top + (entityAABB.height / 2)) - (col->m_tileBounds.top + (col->m_tileBounds.height / 2)));
        float resolve = 0;
        if (std::abs(diffX) >= std::abs(diffY)) {
            if (diffX > 0) {
                resolve = (col->m_tileBounds.left + tileSize) - entityAABB.left;
            } else {
                resolve = -((entityAABB.left + tileSize) - col->m_tileBounds.left);
            }
            l_position->MoveBy(resolve, 0);
            l_collidable->SetPosition(l_position->GetPosition());
            m_systemManager->AddEvent(l_entity, (EventID)EntityEvent::Colliding_X);
            l_collidable->CollideOnX();
        } else {
            if (diffY > 0) {
                resolve = (col->m_tileBounds.top + tileSize) - entityAABB.top;
            } else {
                resolve = -((entityAABB.top + tileSize) - col->m_tileBounds.top);
            }
            l_position->MoveBy(0, resolve);
            l_collidable->SetPosition(l_position->GetPosition());
            m_systemManager->AddEvent(l_entity, (EventID)EntityEvent::Colliding_Y);
            l_collidable->CollideOnY();
        }
    }
}