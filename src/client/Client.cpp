#include "Client.h"

Client::Client() : m_listenThread(&Client::Listen, this), m_connected(false) {}

Client::~Client() {
    Disconnect();
}

bool Client::Connect() {
    if (m_connected) return false;
    sf::Packet packet;
    StampPacket(PacketType::Connect, packet);
    packet << m_playerName;
    if (m_socket.send(packet, m_serverIp, m_serverPort) != sf::Socket::Done) {
        std::cout << "Failed sending a connect packet to server!" << std::endl;
        m_socket.unbind();
        return false;
    }
    packet.clear();
    m_socket.setBlocking(false);
    sf::IpAddress recvIP;
    PortNumber recvPORT;
    sf::Clock timer;
    timer.restart();
    std::cout << "Attempting to connect to server " << m_serverIp << ":" << m_serverPort << std::endl;
    while (timer.getElapsedTime().asMilliseconds() < CONNECT_TIMEOUT) {
        sf::Socket::Status status = m_socket.receive(packet, recvIP, recvPORT);
        if (status != sf::Socket::Done) continue;
        if (recvIP != m_serverIp) {
            std::cout << "A connect packet from unknown server received!" << std::endl;
            continue;
        }
        PacketID pid;
        if (!(packet >> pid)) break;
        if ((PacketType)pid != PacketType::Connect) continue;
        m_packetHandler(pid, packet, this);
        m_connected = true;
        m_lastHeartbeat = m_serverTime;
        m_socket.setBlocking(true);
        m_listenThread.launch();
        return true;
    }
    std::cout << "Failed to connect to server!" << std::endl;
    m_socket.setBlocking(true);
    m_socket.unbind();
    return false;
}

bool Client::Disconnect() {
    if (!m_connected) return false;
    sf::Packet packet;
    StampPacket(PacketType::Disconnect, packet);
    sf::Socket::Status status = m_socket.send(packet, m_serverIp, m_serverPort);
    m_socket.unbind();
    m_connected = false;
    if (status != sf::Socket::Done) return false;
    return true;
}

void Client::Listen() {
    sf::IpAddress recvIP;
    PortNumber recvPORT;
    sf::Packet packet;
    std::cout << "Beginning to listen..." << std::endl;
    while (m_connected) {
        sf::Socket::Status status = m_socket.receive(packet, recvIP, recvPORT);
        if (status != sf::Socket::Done) {
            if (m_connected) {
                std::cout << "Failed receiving a packet from " << recvIP << ":" << recvPORT << std::endl;
                continue;
            } else {
                std::cout << "Socket unbound." << std::endl;
                break;
            }
        }
        if (recvIP != m_serverIp) {
            std::cout << "A packet from unknown server received." << std::endl;
            continue;
        }
        PacketID pid;
        if (!(packet >> pid)) continue;
        PacketType pType = (PacketType)pid;
        if (pType < PacketType::Disconnect || pType >= PacketType::OutOfBounds) {
            std::cout << "Invalid type packet received." << std::endl;
            continue;
        }
        if (pType == PacketType::Heartbeat) {
            sf::Packet p;
            StampPacket(PacketType::Heartbeat, p);
            if (m_socket.send(p, m_serverIp, m_serverPort) != sf::Socket::Done) {
                std::cout << "Failed sending a heartbeat to server." << std::endl;
            }
            sf::Int32 timestamp;
            packet >> timestamp;
            SetTime(sf::milliseconds(timestamp));
            m_lastHeartbeat = m_serverTime;
        } else if (m_packetHandler) {
            m_packetHandler(pid, packet, this);
        }
    }
    std::cout << "Listening stopped..." << std::endl;
}

void Client::Update(const sf::Time& l_time) {
    m_serverTime += l_time;
    if (m_serverTime.asMilliseconds() < 0) {
        m_serverTime -= sf::milliseconds((sf::Int32)Network::HighestTimestamp);
        return;
    }
    if (m_serverTime.asMilliseconds() - m_lastHeartbeat.asMilliseconds() > (sf::Int32)Network::ClientTimeout) {
        Disconnect();
        std::cout << "Client timed out." << std::endl;
    }
}

bool Client::Send(sf::Packet& l_packet) {
    if (!m_connected) return false;
    sf::Socket::Status status = m_socket.send(l_packet, m_serverIp, m_serverPort);
    if (status != sf::Socket::Done) {
        std::cout << "Failed sending a packet to server!" << std::endl;
        return false;
    }
    return true;
}

void Client::Setup(void(*l_handler)(const PacketID&, sf::Packet&, Client*)) {
    m_packetHandler = std::bind(l_handler, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);
}

void Client::SetServerInformation(const sf::IpAddress& l_ip, const PortNumber& l_port) {
    m_serverIp = l_ip;
    m_serverPort = l_port;
}

void Client::UnregisterPacketHandler() {
    m_packetHandler = nullptr;
}

void Client::SetPlayerName(const std::string& l_name) {
    m_playerName = l_name;
}

void Client::SetTime(const sf::Time& l_time) {
    m_serverTime = l_time;
}

const sf::Time& Client::GetTime() const {
    return m_serverTime;
}

const sf::Time& Client::GetLastHeartbeat() const {
    return m_lastHeartbeat;
}

bool Client::IsConnected() const {
    return m_connected;
}

sf::Mutex& Client::GetMutex() {
    return m_mutex;
}