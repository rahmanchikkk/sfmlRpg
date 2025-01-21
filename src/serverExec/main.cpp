#include <SFML/Network.hpp>
#include "Server.h"
#include <iostream>

void HandlePacket(sf::IpAddress& l_ip, const PortNumber& l_port, const PacketID& l_pid, sf::Packet& l_packet, Server* l_server) {
    ClientID cid = l_server->GetClientID(l_ip, l_port);
    if (cid >= 0) {
        if ((PacketType)l_pid == PacketType::Disconnect) {
            sf::Packet p;
            StampPacket(PacketType::Message, p);
            std::string message;
            message = "Client " + std::to_string(cid) + " has left.";
            p << message;
            l_server->Broadcast(p, cid);
        } else if ((PacketType)l_pid == PacketType::Message) {
            sf::Packet p;
            StampPacket(PacketType::Message, p);
            std::string str;
            l_packet >> str;
            std::string message = l_ip.toString() + ":" + std::to_string(l_port) + " " + str;
            p << message;
            l_server->Broadcast(p, cid);
        }
    } else {
        if ((PacketType)l_pid == PacketType::Connect) {
            sf::Packet p;
            StampPacket(PacketType::Connect, p);
            ClientID id = l_server->AddClient(l_ip, l_port);
            l_server->Send(id, p);
        }
    }
}

void CommandProcess(Server* l_server) {
    while (l_server->IsRunning()) {
        std::string str;
        std::getline(std::cin, str);
        if (str == "") continue;
        if (str == "!quit") {
            l_server->Stop();
            break;
        } else if (str == "dc") {
            std::cout << "Disconnecting all clients..." << std::endl;
            l_server->DisconnectAll();
        } else if (str == "clients") {
            std::cout << l_server->GetClientCount() << " clients online" << std::endl;
            std::cout << l_server->GetClientList() << std::endl;
        }
    }
}

int main() {
    Server server(HandlePacket);
    if (server.Start()) {
        sf::Thread c(&CommandProcess, &server);
        c.launch();
        sf::Clock clock;
        clock.restart();
        while (server.IsRunning()) {
            server.Update(clock.restart());
        }
    }
    std::cout << "Listening stopped..." << std::endl;
    return 0;
}