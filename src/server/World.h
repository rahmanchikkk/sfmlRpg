#pragma once
#include "Server.h"
#include "ServerEntityManager.h"
#include "ServerSystemManager.h"
#include "Map.h"
#include "NetSettings.h"

class World {
public:
    World();
    ~World();

    void Update(const sf::Time& l_time);
    void HandlePacket(sf::IpAddress& l_ip, const PortNumber& l_port, const PacketID& l_pid, sf::Packet& l_packet, Server* l_server);
    void ClientLeave(const ClientID& l_client);
    void CommandLine();

    bool IsRunning();
private:
    sf::Time m_tpsTime;
    sf::Time m_serverTime;
    sf::Time m_snapshotTimer;
    sf::Thread m_commandThread;
    Server m_server;
    ServerEntityManager m_entityManager;
    ServerSystemManager m_systemManager;
    Map m_gameMap;
    bool m_running;
    unsigned int m_tps;
    unsigned int m_tick;
};