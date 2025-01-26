#include "Map.h"
#include "StateManager.h"
#include <cmath>

Map::Map(SharedContext* l_context) : m_context(l_context), m_playerId(-1), m_defaultTile(l_context), m_maxMapSize(32, 32) {
    m_context->m_gameMap = this;
    LoadTiles("tiles.cfg");
}

Map::~Map() {
    PurgeMap();
    PurgeTileSet();
    m_context->m_gameMap = nullptr;
}

Tile* Map::GetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer) {
    if (l_x < 0 || l_y < 0 || l_x >= m_maxMapSize.x || l_y >= m_maxMapSize.y || l_layer >= Sheet::Num_Layers) {
        return nullptr;
    }
    auto itr = m_tileMap.find(ConvertCoords(l_x, l_y, l_layer));
    if (itr == m_tileMap.end()) return nullptr;
    return itr->second;
}

TileInfo* Map::GetDefaultTile() {
    return &m_defaultTile;
}

unsigned int Map::GetTileCount() const { return m_tileCount; }
int Map::GetPlayerId() const { return m_playerId; }
const sf::Vector2u& Map::GetMapSize() const { return m_maxMapSize; }
const sf::Vector2f& Map::GetPlayerStart() const { return m_playerStart; }

unsigned int Map::ConvertCoords(unsigned int l_x, unsigned int l_y, unsigned int l_layer) {
    return ((l_layer * m_maxMapSize.y + l_y) * m_maxMapSize.x + l_x);
}

void Map::LoadMap(const std::string& l_mapFile) {
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + l_mapFile);
    if (!file.is_open()) {
        std::cout << "Failed to open map from file: " << l_mapFile << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') continue;
        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "Tile") {
            unsigned int tid = 0;
            keystream >> tid;
            if (tid < 0) {
                std::cout << "Bad tileId: " << tid << std::endl;
                continue;
            }
            auto itr = m_tileSet.find(tid);
            if (itr == m_tileSet.end()) {
                std::cout << "Tile " << tid << " was not found!" << std::endl;
                continue;
            }
            sf::Vector2f tileCoords;
            int tileLayer;
            int tileSolidity;
            keystream >> tileCoords.x >> tileCoords.y >> tileLayer >> tileSolidity;
            Tile* tile = nullptr;
            tile->m_properties = itr->second;
            tile->m_solid = tileSolidity;
            if (!m_tileMap.emplace(ConvertCoords(tileCoords.x, tileCoords.y, tileLayer), tile).second) {
                std::cout << "Duplicate tile: " << tid << std::endl;
                delete tile;
                continue;
            }
            std::string warp;
            keystream >> warp;
            tile->m_warp = false;
            if (warp == "WARP") tile->m_warp = true;
        } else if (type == "SIZE") {
            keystream >> m_maxMapSize.x >> m_maxMapSize.y;
        } else if (type == "DEFAULT_FRICTION") {
            keystream >> m_defaultTile.m_friction.x >> m_defaultTile.m_friction.y;
        } else if (type == "ENTITY") {
            std::string name;
            keystream >> name;
            if (name == "Player" && m_playerId != -1) continue;
            int entityId = m_context->m_entityManager->AddEntity(name);
            if (entityId == -1) continue;
            if (name == "Player") m_playerId = entityId;
            C_Base* pos = m_context->m_entityManager->GetComponent<C_Position>(entityId, Component::Position);
            if (pos) keystream >> *pos;
        } else {
            std::cout << "Unknown type: " << type << std::endl;
        }
    }
    file.close();
}

void Map::LoadTiles(const std::string& l_tilesFile) {
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + l_tilesFile);
    if (!file.is_open()) {
        std::cout << "Failed to open tiles from file: " << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') continue;
        std::stringstream keystream(line);
        TileID tId;
        keystream >> tId;
        if (tId < 0) continue;
        TileInfo* info = new TileInfo(m_context, "TileSheet", tId);
        keystream >> info->m_name >> info->m_friction.x >> info->m_friction.y >> info->m_deadly;
        if (!m_tileSet.emplace(tId, info).second) {
            std::cout << "Duplicate tile: " << tId << std::endl;
            delete info;
            info = nullptr;
        }
    }
    file.close();
}

void Map::Update(float l_dT) {}

void Map::Draw(unsigned int l_layer) {
    if (l_layer >= Sheet::Num_Layers) return;
    sf::RenderWindow* wind = m_context->m_window->GetRenderWindow();
    sf::FloatRect viewSpace = m_context->m_window->GetViewSpace();
    sf::Vector2f tileBegin(std::floor(viewSpace.left / Sheet::Tile_Size), std::floor(viewSpace.top / Sheet::Tile_Size));
    sf::Vector2f tileEnd(std::ceil((viewSpace.left + viewSpace.width) / Sheet::Tile_Size), std::ceil((viewSpace.top + viewSpace.height) / Sheet::Tile_Size));
    int count = 0;
    for (int x = tileBegin.x; x <= tileEnd.x; ++x) {
        for (int y = tileBegin.y; y <= tileEnd.y; ++y) {
            Tile* tile = GetTile(x, y, l_layer);
            if (!tile) continue;
            sf::Sprite& sprite = tile->m_properties->m_sprite;
            sprite.setPosition(x * Sheet::Tile_Size, y * Sheet::Tile_Size);
            wind->draw(sprite);
            ++count;
            if (!m_context->m_debugOverlay->Debug()) continue;
            if (!tile->m_properties->m_deadly && !tile->m_solid && !tile->m_warp) continue;
            sf::RectangleShape* tileMarker = new sf::RectangleShape(sf::Vector2f(Sheet::Tile_Size, Sheet::Tile_Size));
            tileMarker->setPosition(x * Sheet::Tile_Size, y * Sheet::Tile_Size);
            if (tile->m_properties->m_deadly) {
                tileMarker->setFillColor(sf::Color(255, 0, 0, 100));
            } else if (tile->m_solid) {
                tileMarker->setFillColor(sf::Color(0, 255, 0, 150));
            } else if (tile->m_warp) {
                tileMarker->setFillColor(sf::Color(0, 0, 255, 150));
            }
            m_context->m_debugOverlay->Add(tileMarker);
        }
    }
}

void Map::PurgeMap() {
    while (m_tileMap.begin() != m_tileMap.end()) {
        delete m_tileMap.begin()->second;
        m_tileMap.erase(m_tileMap.begin());
    }
    m_tileCount = 0;
    m_context->m_entityManager->Purge();
}

void Map::PurgeTileSet() {
    while (m_tileSet.begin() != m_tileSet.end()) {
        delete m_tileSet.begin()->second;
        m_tileSet.erase(m_tileSet.begin());
    }
    m_tileSetCount = 0;
}