#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <string.h>
#include <SFML/Network.hpp>

#define MAX_RAW_DATA 256 //Max bytes supported on Raw Data mode

#define logl(x) std::cout << x << std::endl
#define log(x) std::cout << x

class ServerNetwork{
     sf::TcpListener listener;
     std::vector<sf::TcpSocket *> client_array;

     unsigned short listen_port;
     bool rawMode = false;
public:
     ServerNetwork(unsigned short, bool);
     void ConnectClients(std::vector<sf::TcpSocket *> *);
     void DisconnectClient(sf::TcpSocket *, size_t);

     void ReceivePacket(sf::TcpSocket *, size_t);
     void ReceiveRawData(sf::TcpSocket *, size_t);

     void BroadcastPacket(sf::Packet &, sf::IpAddress, unsigned short);
     void BroadcastRawData(const char*, sf::IpAddress, unsigned short);

     void ManagePackets();
     void Run();
};
