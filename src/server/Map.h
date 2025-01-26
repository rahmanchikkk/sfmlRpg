#pragma once
#include <SFML/Graphics.hpp>
#include "SharedContext.h"
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <map>
#include "Utilities.h"

enum Sheet { Tile_Size = 32, Sheet_Width = 256, Sheet_Height = 256, Num_Layers = 4 };
using TileID = unsigned int;

struct TileInfo {
    TileInfo(SharedContext* l_context, const std::string& l_texture = "", TileID l_id = 0) : m_context(l_context), m_id(0), m_deadly(false) {
        TextureManager* textureManager = m_context->m_textureManager;
        if (l_texture == "") { m_id = l_id; return; }
        if (!textureManager->RequireResource(l_texture)) return;
        m_texture = l_texture;
        m_id = l_id;
        m_sprite.setTexture(*textureManager->GetResource(m_texture));
        sf::IntRect tileBoundaries(m_id % (Sheet::Sheet_Width / Sheet::Tile_Size), m_id / (Sheet::Sheet_Height / Sheet::Tile_Size), 
            Sheet::Tile_Size, Sheet::Tile_Size);
        m_sprite.setTextureRect(tileBoundaries);
    };

    ~TileInfo() {
        if (m_texture == "") return;
        m_context->m_textureManager->ReleaseResource(m_texture);
    };

    SharedContext* m_context;
    sf::Sprite m_sprite;
    std::string m_texture;
    TileID m_id;
    std::string m_name;
    bool m_deadly;
    sf::Vector2f m_friction;
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
    Map(SharedContext* m_context);
    ~Map();

    TileInfo* GetDefaultTile();
    Tile* GetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer);

    unsigned int GetTileCount() const;
    const sf::Vector2f& GetPlayerStart() const;
    const sf::Vector2u& GetMapSize() const;
    int GetPlayerId() const;

    void LoadMap(const std::string& l_mapFile);
    void Update(float l_dT);
    void Draw(unsigned int l_layer);
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

    int m_playerId;
    unsigned int m_tileCount;
    unsigned int m_tileSetCount;
    SharedContext* m_context;
};