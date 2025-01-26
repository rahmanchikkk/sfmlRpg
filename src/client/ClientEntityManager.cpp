#include "ClientEntityManager.h"
#include "SystemManager.h"

ClientEntityManager::ClientEntityManager(SystemManager* l_sysMgr, TextureManager* l_textureMgr) : EntityManager(l_sysMgr), m_textureManager(l_textureMgr) {
    AddComponentType<C_Position>(Component::Position);
    AddComponentType<C_State>(Component::State);
    AddComponentType<C_SpriteSheet>(Component::SpriteSheet);
    AddComponentType<C_Movable>(Component::Movable);
    AddComponentType<C_Collidable>(Component::Collidable);
    AddComponentType<C_Controller>(Component::Controller);
    AddComponentType<C_SoundEmitter>(Component::SoundEmitter);
    AddComponentType<C_SoundListener>(Component::SoundListener);
    AddComponentType<C_Client>(Component::Client);
    AddComponentType<C_Name>(Component::Name);
    AddComponentType<C_Health>(Component::Health);
    AddComponentType<C_UI_Element>(Component::UI_Element);
}

ClientEntityManager::~ClientEntityManager() {}

int ClientEntityManager::AddEntity(const std::string& l_entityFile, int l_id = -1) {
    EntityId entityId;
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + "media/Entities" + l_entityFile + ".entity");
    if (!file.is_open()) {
        std::cout << "Failed opening entity from file: " << l_entityFile << std::endl;
        return -1;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') continue;
        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "Name") {

        } else if (type == "Entity") {
            if (entityId != -1) continue;
            Bitset set = 0;
            Bitmask mask;
            keystream >> set;
            mask.SetMask(set);
            
        }
    }
}