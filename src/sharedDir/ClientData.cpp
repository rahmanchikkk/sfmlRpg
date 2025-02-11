#include "ClientData.h"

sf::Packet& operator <<(sf::Packet& l_packet, const ClientData& l_data) {
    l_packet << l_data.m_id << l_data.m_nickname << l_data.m_password << l_data.m_email << l_data.m_gold << (int)l_data.m_skins.size();
    for (int i = 0; i < (int)l_data.m_skins.size(); ++i) {
        l_packet << l_data.m_skins[i];
    }
    return l_packet;
}

sf::Packet& operator >>(sf::Packet& l_packet, ClientData& l_data) {
    l_packet >> l_data.m_id >> l_data.m_nickname >> l_data.m_password >> l_data.m_email >> l_data.m_gold;
    int n;
    l_packet >> n;
    l_data.m_skins = std::vector<int>(n);
    for (int i = 0; i < n; ++i) {
        l_packet >> l_data.m_skins[i];
    }
    return l_packet;
}