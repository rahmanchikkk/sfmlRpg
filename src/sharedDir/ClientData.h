#pragma once
#include <SFML/Network/Packet.hpp>

struct ClientData {
    std::string m_id;
    std::string m_nickname;
    std::string m_email;
    std::string m_password;
    unsigned int m_gold;
    std::vector<int> m_skins;
};

sf::Packet& operator <<(sf::Packet& l_packet, const ClientData& l_data);
sf::Packet& operator >>(sf::Packet& l_packet, ClientData& l_data);