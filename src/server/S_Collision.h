#pragma once
#include <SFML/Graphics/Rect.hpp>
#include "S_Base.h"
#include "C_Position.h"
#include "C_Collidable.h"

struct TileInfo;
class Map;

struct CollisionElement {
    CollisionElement(float l_area, TileInfo* l_info, const sf::FloatRect& l_tileBounds) :
        m_area(l_area), m_tile(l_info), m_tileBounds(l_tileBounds) {}; 
    float m_area;
    TileInfo* m_tile;
    sf::FloatRect m_tileBounds;
};

using Collisions = std::vector<CollisionElement*>;

class S_Collision : public S_Base {
public:
    S_Collision(SystemManager* l_sysMgr);
    ~S_Collision();
    
    void Update(float l_dT);
    void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
    void Notify(const Message& l_msg);
private:
    void EntityCollisions();
    void MapCollisions(const EntityId& l_entity, C_Position* l_position, C_Collidable* l_collidable);
    void CheckOutOfBounds(C_Position* l_position, C_Collidable* l_collidable);

    Map* m_gameMap;
};