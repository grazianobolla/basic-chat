#include "servernetwork.hpp"

ServerNetwork::ServerNetwork(unsigned short port, bool rawMode = false) : listen_port(port){
     logl("Chat Server Started");

     this->rawMode = rawMode;
     if(rawMode) logl("Warning, raw mode enabled");

     if(listener.listen(listen_port) != sf::Socket::Done){
          logl("Could not listen");
     }
}

void ServerNetwork::ConnectClients(std::vector<sf::TcpSocket *> * client_array){
     while(true){
          logl("Listening new connections");
          sf::TcpSocket * new_client = new sf::TcpSocket();

          if(listener.accept(*new_client) == sf::Socket::Done){
               new_client->setBlocking(false);
               client_array->push_back(new_client);
               logl("Added client " << new_client->getRemoteAddress() << ":" << new_client->getRemotePort() << " on slot " << client_array->size());
          }else logl("Could not add the new client");
     }
}

void ServerNetwork::DisconnectClient(sf::TcpSocket * socket_pointer, size_t position){
     logl("Client " << socket_pointer->getRemoteAddress() << ":" << socket_pointer->getRemotePort() << " disconnected, removing");
     socket_pointer->disconnect();
     delete(socket_pointer);
     client_array.erase(client_array.begin() + position);
}

void ServerNetwork::BroadcastPacket(sf::Packet & packet, sf::IpAddress exclude_address, unsigned short port){
     for(size_t iterator = 0; iterator < client_array.size(); iterator++){
          sf::TcpSocket * client = client_array[iterator];
          if(client->getRemoteAddress() != exclude_address || client->getRemotePort() != port){
               if(client->send(packet) != sf::Socket::Done){
                    logl("Could not send packet on broadcast");
               }
          }
     }
}

void ServerNetwork::BroadcastPacket(const char * data, sf::IpAddress exclude_address, unsigned short port){
     for(size_t iterator = 0; iterator < client_array.size(); iterator++){
          sf::TcpSocket * client = client_array[iterator];
          if(client->getRemoteAddress() != exclude_address || client->getRemotePort() != port){
               if(client->send(data, sizeof(data)) != sf::Socket::Done){
                    logl("Could not send packet on broadcast");
               }
          }
     }
}

void ServerNetwork::ReceiveRawData(sf::TcpSocket * client, size_t iterator){
     char received_data[256]; size_t received_bytes;
     memset(received_data, 0, sizeof(received_data));
     if(client->receive(received_data, sizeof(received_data), received_bytes) == sf::Socket::Disconnected){
          DisconnectClient(client, iterator);
     }else{
          if(received_bytes > 0){
               BroadcastPacket(received_data, client->getRemoteAddress(), client->getRemotePort());
               logl(client->getRemoteAddress().toString() << ":" << client->getRemotePort() << " '" << received_data << "'");
          }
     }
}

void ServerNetwork::ReceivePacket(sf::TcpSocket * client, size_t iterator){
     sf::Packet packet;
     if(client->receive(packet) == sf::Socket::Disconnected){
          DisconnectClient(client, iterator);
     }else{
          if(packet.getDataSize() > 0){
               std::string received_message;
               packet >> received_message;
               packet.clear();

               packet << received_message << client->getRemoteAddress().toString() << client->getRemotePort();

               BroadcastPacket(packet, client->getRemoteAddress(), client->getRemotePort());
               logl(client->getRemoteAddress().toString() << ":" << client->getRemotePort() << " '" << received_message << "'");
          }
     }
}

void ServerNetwork::ManagePackets(){
     while(true){
          for(size_t iterator = 0; iterator < client_array.size(); iterator++){
               if(rawMode == true) ReceiveRawData(client_array[iterator], iterator);
               else ReceivePacket(client_array[iterator], iterator);
          }
     }
}

void ServerNetwork::Run(){
     std::thread connetion_thread(&ServerNetwork::ConnectClients, this, &client_array);

     ManagePackets();
}
