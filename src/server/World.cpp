#include "World.h"

World::World() : m_server(&World::HandlePacket, this), m_commandThread(&World::CommandLine, this), 
    m_tick(0), m_tps(0), m_running(false), m_entityManager(nullptr), m_gameMap(&m_entityManager) {
    if (!m_server.Start()) return;
    m_running = true;
    m_server.BindTimeoutHandler(&World::ClientLeave, this);
    m_gameMap.LoadMap("media/Maps/map1.map");
    m_systemManager.SetEntityManager(&m_entityManager);
    m_entityManager.SetSystemManager(&m_systemManager);
    m_systemManager.GetSystem<S_Collision>(System::Collision)->SetMap(&m_gameMap);
    m_systemManager.GetSystem<S_Movement>(System::Movement)->SetMap(&m_gameMap);
    m_systemManager.GetSystem<S_Network>(System::Network)->RegisterServer(&m_server);
    m_commandThread.launch();
}

World::~World() {
    m_entityManager.SetSystemManager(nullptr);
}

void World::Update(const sf::Time& l_time) {
    if (!m_server.IsRunning()) { m_running = false; return; }
    m_serverTime += l_time;
    m_tpsTime += l_time;
    m_snapshotTimer += l_time;
    m_server.GetMutex().lock();
    m_systemManager.Update(l_time.asMilliseconds());
    m_server.GetMutex().unlock();
    if (m_snapshotTimer.asMilliseconds() >= SNAPSHOT_INTERVAL) {
        sf::Packet packet;
        m_systemManager.GetSystem<S_Network>(System::Network)->CreateSnapshot(packet);
        m_server.Broadcast(packet);
        m_snapshotTimer = sf::milliseconds(0);
    }
    if (m_tpsTime >= sf::milliseconds(1000)) {
        m_tps = m_tick;
        m_tpsTime = sf::milliseconds(0);
        m_tick = 0;
    } else {
        ++m_tick;
    }
}

void World::HandlePacket(sf::IpAddress& l_ip, const PortNumber& l_port, const PacketID& l_pid, sf::Packet& l_packet, Server* l_server) {
    ClientID cid = l_server->GetClientID(l_ip, l_port);
    PacketType pType = (PacketType)l_pid;
    if (cid >= 0) {
        if (pType == PacketType::Disconnect) {
            ClientLeave(cid);
            l_server->RemoveClient(cid);
        } else if (pType == PacketType::Message) {

        } else if (pType == PacketType::PlayerUpdate) {
            m_systemManager.GetSystem<S_Network>(System::Network)->UpdatePlayer(l_packet, cid);
        }
    } else {
        if (pType != PacketType::Connect) return;
        std::string nickname;
        if (!(l_packet >> nickname)) return;
        ClientID id = l_server->AddClient(l_ip, l_port);
        if (id < 0) {
            sf::Packet p;
            StampPacket(PacketType::Disconnect, p);
            l_server->Send(l_ip, l_port, p);
            return;
        }
        sf::Lock lock(l_server->GetMutex());
        sf::Int32 eid = m_entityManager.AddEntity("Player");
        if (eid == -1) return;
        m_systemManager.GetSystem<S_Network>(System::Network)->RegisterClientID(eid, id);
        C_Position* pos = m_entityManager.GetComponent<C_Position>(eid, Component::Position);
        pos->SetPosition(64.f, 64.f);
        m_entityManager.GetComponent<C_Name>(eid, Component::Name)->SetName(nickname);
        sf::Packet p;
        StampPacket(PacketType::Connect, p);
        p << eid;
        p << pos->GetPosition().x << pos->GetPosition().y;
        if (!l_server->Send(id, p)) {
            std::cout << "Unable to send a connect packet to client: " << id << std::endl;
            return;
        }
    }
}

void World::ClientLeave(const ClientID& l_client) {
    sf::Lock lock(m_server.GetMutex());
    S_Network* net = m_systemManager.GetSystem<S_Network>(System::Network);
    net->RemoveEntity(net->GetEntityId(l_client));
}

void World::CommandLine() {
    while (m_server.IsRunning()) {
        std::string type;
        std::getline(std::cin, type);
        if (type == "terminate") {
            m_server.Stop();
            m_running = false;
            return;
        } else if (type == "disconnectall") {
            std::cout << "Disconnecting all clients..." << std::endl;
            m_server.DisconnectAll();
            sf::Lock lock(m_server.GetMutex());
            m_entityManager.Purge();
        } else if (type.find("tps") != std::string::npos) {
            std::cout << "TPS: " << m_tps << std::endl;
        } else if (type.find("health") != std::string::npos) {
            std::stringstream keystream(type);
            std::string command;
            std::string eid;
            std::string health;
            if (!(keystream >> command)) continue;
            if (!(keystream >> eid)) continue;
            if (!(keystream >> health)) continue;
            EntityId id = std::stoi(eid);
            Health healthValue = std::stoi(health);
            C_Health* h = m_entityManager.GetComponent<C_Health>(id, Component::Health);
            if (!h) continue;
            h->SetHealth(healthValue);
        } else if (type == "clients") {
            std::cout << m_server.GetClientCount() << " clients online." << std::endl;
            std::cout << m_server.GetClientList() << std::endl;
        } else if (type == "entities") {
            std::cout << "Current entity count: " << m_entityManager.GetEntityCount() << std::endl;
        }
    }
}

bool World::IsRunning() { return m_running; }