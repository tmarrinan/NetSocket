#include <iostream>
#include "netsocket/server.h"

void OnConnection(NetSocket::Server& server, NetSocket::ClientConnection::Pointer client);
void OnDisonnect(NetSocket::Server& server, std::string client_endpoint);
void OnMessage(NetSocket::Server& server, NetSocket::ClientConnection::Pointer client, void *data, uint32_t length);

int main(int argc, char **argv)
{
    uint16_t port = 8000;

    // create server
    NetSocket::Server server(port);
    
    // add callback functions
    server.ConnectCallback(OnConnection);
    server.DisconnectCallback(OnDisonnect);
    
    // run server indefinitely
    server.Run();

    return 0;
}

void OnConnection(NetSocket::Server& server, NetSocket::ClientConnection::Pointer client)
{
    std::cout << "New Connection: " << client->Endpoint() << std::endl;
    client->ReadCallback(OnMessage);

    client->Write("Hello there!!!\n");
}

void OnDisonnect(NetSocket::Server& server, std::string client_endpoint)
{
    std::cout << "Client Disconnect: " << client_endpoint << std::endl;
}

void OnMessage(NetSocket::Server& server, NetSocket::ClientConnection::Pointer client, void *data, uint32_t length)
{
    std::string message = std::string((char*)data, length);
    std::cout << message << std::endl;
}