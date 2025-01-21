#pragma once
#include <SFML/Network.hpp>
#include "NetworkDefinitions.h"
#include "PacketTypes.h"
#include <functional>
#include <iostream>
#define CONNECT_TIMEOUT 5000

class Client;

using PacketHandler = std::function<void(const PacketID&, sf::Packet&, Client*)>;

class Client {
public:
    Client();
    ~Client();

    bool Connect();
    bool Disconnect();
    void Listen();
    void Update(const sf::Time& l_time);
    bool Send(sf::Packet& l_packet);

    template <class T>
    void Setup(void(T::*l_handler)(const PacketID&, sf::Packet&, Client*), T* l_instance) {
        m_packetHandler = std::bind(l_handler, l_instance, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
    };
    void Setup(void(*l_handler)(const PacketID&, sf::Packet&, Client*));

    void SetServerInformation(const sf::IpAddress& l_ip, const PortNumber& l_port);
    void UnregisterPacketHandler();

    void SetPlayerName(const std::string& l_name);
    void SetTime(const sf::Time& l_time);
    const sf::Time& GetTime() const;
    const sf::Time& GetLastHeartbeat() const;

    bool IsConnected() const;
    sf::Mutex& GetMutex();
private:
    sf::UdpSocket m_socket;
    sf::IpAddress m_serverIp;
    PortNumber m_serverPort;
    PacketHandler m_packetHandler;

    sf::Time m_serverTime;
    sf::Time m_lastHeartbeat;

    std::string m_playerName;
    bool m_connected;

    sf::Thread m_listenThread;
    sf::Mutex m_mutex;
};