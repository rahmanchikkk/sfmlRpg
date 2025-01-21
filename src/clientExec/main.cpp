#include <SFML/Network.hpp>
#include "Client.h"
#include <iostream>

void HandlePacket(const PacketID& l_pid, sf::Packet& l_packet, Client* l_client) {
    if ((PacketType)l_pid == PacketType::Message) {
        std::string message;
        l_packet >> message;
        std::cout << message << std::endl;
    } else if ((PacketType)l_pid == PacketType::Disconnect) {
        l_client->Disconnect();
    }
}

void CommandProcess(Client* l_client) {
    while (l_client->IsConnected()) {
        std::string str;
        std::getline(std::cin, str);
        if (str != "") {
            if (str == "!quit") {
                l_client->Disconnect();
                break;
            }
            sf::Packet p;
            StampPacket(PacketType::Message, p);
            p << str;
            l_client->Send(p);
        }
    }
}

int main(int argc, char** argv) {
    sf::IpAddress serverIp;
    PortNumber serverPort;
    if (argc == 1) {
        std::cout << "Enter server IP: ";
        std::cin >> serverIp;
        std::cout << "Enter server PORT: ";
        std::cin >> serverPort;
    } else if (argc == 3) {
        serverIp = argv[1];
        serverPort = atoi(argv[2]);
    } else {
        return 0;
    }
    Client client;
    client.SetServerInformation(serverIp, serverPort);
    client.Setup(&HandlePacket);
    sf::Thread c(&CommandProcess, &client);
    if (client.Connect()) {
        c.launch();
        sf::Clock timer;
        timer.restart();
        while (client.IsConnected()) {
            client.Update(timer.restart());
        }
    } else {
        std::cout << "Failed to connect to server." << std::endl;
    }
    return 0;
}