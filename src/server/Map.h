#pragma once
#include <SFML/Graphics.hpp>
#include "ServerEntityManager.h"
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <map>
#include "Utilities.h"

enum Sheet { Tile_Size = 32, Sheet_Width = 256, Sheet_Height = 256, Num_Layers = 4 };
using TileID = unsigned int;

struct TileInfo {
    TileInfo(TileID l_id = 0) { 
        m_id = l_id;
        m_deadly = false;
    };

    ~TileInfo() {};

    TileID m_id;
    sf::Vector2f m_friction;
    std::string m_name;
    bool m_deadly;
};

struct Tile {
    TileInfo* m_properties;
    bool m_solid;
    bool m_warp;
};

using TileMap = std::unordered_map<TileID, Tile*>;
using TileSet = std::unordered_map<TileID, TileInfo*>;

class Map {
public:
    Map(ServerEntityManager* l_entityMgr);
    ~Map();

    TileInfo* GetDefaultTile();
    Tile* GetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer);

    unsigned int GetTileCount() const;
    const sf::Vector2f& GetPlayerStart() const;
    const sf::Vector2u& GetMapSize() const;

    void LoadMap(const std::string& l_mapFile);
    void Update(float l_dT);
private:
    unsigned int ConvertCoords(unsigned int l_x, unsigned int l_y, unsigned int l_layer);
    void LoadTiles(const std::string& l_tilesFile);
    void PurgeMap();
    void PurgeTileSet();

    TileMap m_tileMap;
    TileSet m_tileSet;
    TileInfo m_defaultTile;
    
    sf::Vector2u m_maxMapSize;
    sf::Vector2f m_playerStart;

    unsigned int m_tileCount;
    unsigned int m_tileSetCount;
    ServerEntityManager* m_entityManager;
};