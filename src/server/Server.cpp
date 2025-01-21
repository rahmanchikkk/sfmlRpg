#include "Server.h"

Server::Server(void(*l_handler)(sf::IpAddress&, const PortNumber&, const PacketID&, sf::Packet&, Server*)) :
    m_listenThread(&Server::Listen, this), m_running(false) {
        m_packetHandler = std::bind(l_handler, std::placeholders::_1, std::placeholders::_2, 
            std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    }

Server::~Server() { 
    Stop();
}

void Server::BindTimeoutHandler(void(*l_handler)(const ClientID&)) {
    m_timeoutHandler = std::bind(l_handler, std::placeholders::_1);
}

bool Server::Send(const ClientID& l_client, sf::Packet& l_packet) {
    sf::Lock lock(m_mutex);
    auto itr = m_clients.find(l_client);
    if (itr == m_clients.end()) return false;
    if (m_outgoing.send(l_packet, itr->second.m_clientIP, itr->second.m_clientPORT) != sf::Socket::Done) {
        std::cout << "Failed sending a packet to client: " << itr->first << std::endl;
        return false;
    }
    m_totalSent += l_packet.getDataSize();
    return true;
}

bool Server::Send(sf::IpAddress& l_ip, const PortNumber& l_port, sf::Packet& l_packet) {
    if (m_outgoing.send(l_packet, l_ip, l_port) != sf::Socket::Done) return false;
    m_totalSent += l_packet.getDataSize();
    return true;
}

void Server::Listen() {
    sf::IpAddress recvIP;
    PortNumber recvPORT;
    sf::Packet packet;
    std::cout << "Beginning to listen..." << std::endl;
    while (m_running) {
        packet.clear();
        sf::Socket::Status status = m_incoming.receive(packet, recvIP, recvPORT);
        if (status != sf::Socket::Done) {
            if (m_running) {
                std::cout << "Failed receiving a packet from: " << recvIP << ":" << recvPORT << std::endl;
                continue; 
            } else {
                std::cout << "Socket unbound." << std::endl;
                break;
            }
        }
        m_totalReceived += packet.getDataSize();
        PacketID pid;
        if (!(packet >> pid)) continue;
        PacketType pType = (PacketType)pid;
        if (pType < PacketType::Disconnect || pType >= PacketType::OutOfBounds) {
            std::cout << "Invalid type packet received." << std::endl;
            continue;
        }
        if (pType == PacketType::Heartbeat) {
            bool clientFound = false;
            sf::Lock lock(m_mutex);
            for (auto& itr : m_clients) {
                if (itr.second.m_clientIP != recvIP || itr.second.m_clientPORT != recvPORT) continue;
                clientFound = true;
                if (!itr.second.m_heartbeatWaiting) {
                    std::cout << "Invalid heartbeat packet received." << std::endl;
                    break;
                }
                itr.second.m_latency = m_serverTime.asMilliseconds() - itr.second.m_lastHeartbeat.asMilliseconds();
                itr.second.m_heartbeatRetry = 0;
                itr.second.m_lastHeartbeat = m_serverTime;
                itr.second.m_heartbeatWaiting = false;
                break;
            }
            if (!clientFound) std::cout << "Unknown client!" << std::endl;
        } else if (m_packetHandler) {
            m_packetHandler(recvIP, recvPORT, pid, packet, this);
        }
    }
    std::cout << "Listening stopped..." << std::endl;
}

void Server::Update(const sf::Time& l_time) {
    m_serverTime += l_time;
    if (m_serverTime.asMilliseconds() < 0) {
        m_serverTime -= sf::milliseconds((sf::Int32)Network::HighestTimestamp);
        sf::Lock lock(m_mutex);
        for (auto& itr : m_clients) {
            itr.second.m_lastHeartbeat = sf::milliseconds(std::abs(itr.second.m_lastHeartbeat.asMilliseconds() - 
                (sf::Int32)Network::HighestTimestamp));
        }
        return;
    }
    sf::Lock lock(m_mutex);
    for (auto itr = m_clients.begin(); itr != m_clients.end(); ) {
        sf::Int32 elapsed = m_serverTime.asMilliseconds() - itr->second.m_lastHeartbeat.asMilliseconds();
        if (elapsed >= HEARTBEAT_INTERVAL) {
            if (elapsed >= (sf::Int32)Network::ClientTimeout) {
                std::cout << "Client " << itr->first << " has timed out." << std::endl;
                if (m_timeoutHandler) m_timeoutHandler(itr->first);
                itr = m_clients.erase(itr);
                continue;
            }
            if (!itr->second.m_heartbeatWaiting && (elapsed >= ((itr->second.m_heartbeatRetry + 1) * HEARTBEAT_INTERVAL))) {
                if (itr->second.m_heartbeatRetry >= 3) {
                    std::cout << "Retry(" << itr->second.m_heartbeatRetry << ") for client: " << itr->first << std::endl;
                }
                ++itr->second.m_heartbeatRetry;
                sf::Packet packet;
                StampPacket(PacketType::Heartbeat, packet);
                packet << sf::Int32(m_serverTime.asMilliseconds());
                Send(itr->first, packet);
                if (itr->second.m_heartbeatRetry == 0) {
                    itr->second.m_heartbeatSent = m_serverTime;
                }
                itr->second.m_heartbeatWaiting = true;
                ++itr->second.m_heartbeatRetry;
                m_totalSent += packet.getDataSize();
            }
        }
        ++itr;
    }
} 

void Server::Broadcast(sf::Packet& l_packet, const ClientID& l_ignore) {
    sf::Lock lock(m_mutex);
    for (auto& itr : m_clients) {
        if (itr.first == l_ignore) continue;
        if (m_outgoing.send(l_packet, itr.second.m_clientIP, itr.second.m_clientPORT) != sf::Socket::Done) {
            std::cout << "Failed sending a packet to client: " << itr.first << std::endl;
            continue;
        }
        m_totalSent += l_packet.getDataSize();
    }
}

ClientID Server::AddClient(const sf::IpAddress& l_ip, const PortNumber& l_port) {
    sf::Lock lock(m_mutex);
    if (HasClient(l_ip, l_port)) {
        return (ClientID)Network::NullID;
    }
    ClientID cid = m_lastID;
    ClientInfo info(l_ip, l_port, m_serverTime);
    if (!m_clients.emplace(cid, info).second) {
        return (ClientID)Network::NullID;
    }
    ++m_lastID;
    return cid;
}

ClientID Server::GetClientID(const sf::IpAddress& l_ip, const PortNumber& l_port) {
    sf::Lock lock(m_mutex);
    for (auto& itr : m_clients) {
        if (itr.second.m_clientIP == l_ip && itr.second.m_clientPORT == l_port) {
            return itr.first;
        }
    }
    return (ClientID)Network::NullID;
}

bool Server::GetClientInfo(const ClientID& l_client, ClientInfo& l_info) {
    sf::Lock lock(m_mutex);
    auto itr = m_clients.find(l_client);
    if (itr == m_clients.end()) return false;
    l_info = itr->second;
    return true;
}

bool Server::RemoveClient(const ClientID& l_client) {
    sf::Lock lock(m_mutex);
    auto itr = m_clients.find(l_client);
    if (itr == m_clients.end()) return false;
    sf::Packet packet;
    StampPacket(PacketType::Disconnect, packet);
    Send(l_client, packet);
    m_clients.erase(itr);
    return true;
}

bool Server::RemoveClient(const sf::IpAddress& l_ip, const PortNumber& l_port) {
    sf::Lock lock(m_mutex);
    for (auto itr = m_clients.begin(); itr != m_clients.end(); ) {
        if (itr->second.m_clientIP == l_ip && itr->second.m_clientPORT == l_port) {
            sf::Packet packet;
            StampPacket(PacketType::Disconnect, packet);
            Send(itr->first, packet);
            m_clients.erase(itr);
            return true;
        }
        ++itr;
    }
    return false;
}

bool Server::HasClient(const ClientID& l_client) {
    return (m_clients.find(l_client) != m_clients.end());
}

bool Server::HasClient(const sf::IpAddress& l_ip, const PortNumber& l_port) {
    sf::Lock lock(m_mutex);
    for (auto& itr : m_clients) {
        if (itr.second.m_clientIP == l_ip && itr.second.m_clientPORT == l_port) {
            return true;
        }
    }
    return false;
}

void Server::DisconnectAll() {
    if (!m_running) return;
    sf::Packet packet;
    StampPacket(PacketType::Disconnect, packet);
    Broadcast(packet);
    sf::Lock lock(m_mutex);
    m_clients.clear();
}

bool Server::Start() {
    if (m_running) return false;
    m_outgoing.bind(sf::Socket::AnyPort);
    if (m_incoming.bind((PortNumber)Network::ServerPort) != sf::Socket::Done) return false;
    Setup();
    std::cout << "Incoming port: " << m_incoming.getLocalPort() << ", outgoing port: " << m_outgoing.getLocalPort() << std::endl;
    m_running = true;
    m_listenThread.launch();
    return true;
}

bool Server::Stop() {
    if (!m_running) return false;
    DisconnectAll();
    m_incoming.unbind();
    m_running = false;
    return true;
}

bool Server::IsRunning() {
    return m_running;
}

unsigned int Server::GetClientCount() {
    return (unsigned int)m_clients.size();
}

sf::Mutex& Server::GetMutex() {
    return m_mutex;
}

std::string Server::GetClientList() {
    std::string delimeter = "------------------------------";
    std::string list;
    list = delimeter;
    list += '\n';
    list += "ID";
    list += '\t';
    list += "Client IP:PORT";
    list += '\t'; list += '\t';
    list += "Ping";
    list += '\n';
    list += delimeter;
    list += '\n';
    for (auto itr = m_clients.begin(); itr != m_clients.end(); ++itr) {
        list += std::to_string(itr->first);
        list += '\t';
        list += itr->second.m_clientIP.toString() + ":" + std::to_string(itr->second.m_clientPORT);
        list += '\t';
        list += std::to_string(itr->second.m_latency);
        list += '\n';
    }
    list += delimeter;
    list += '\n';
    list += "Total data sent: " + std::to_string(m_totalSent / 1000) + " kB. Total data received: " + 
        std::to_string(m_totalReceived / 1000) + " kB.";
    list += '\n';
    return list;
}

void Server::Setup() {
    m_lastID = 0;
    m_running = false;
    m_totalReceived = 0;
    m_totalSent = 0;
}