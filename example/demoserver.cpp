#include <iostream>
#include "netsocket/server.h"

void OnConnection(NetSocket::ClientConnection::pointer client);
void OnMessage(NetSocket::ClientConnection::pointer client, void *data, uint32_t length);

int main(int argc, char **argv)
{
    uint16_t port = 8000;

    // create server
    NetSocket::Server server(port);
    
    // add connect callback function
    server.ConnectCallback(OnConnection);
    
    // run server indefinitely
    server.Run();

    return 0;
}

void OnConnection(NetSocket::ClientConnection::pointer client)
{
    std::cout << "New Connection: " << client->IpAddress() << ":" << client->Port() << std::endl;
    client->ReadCallback(OnMessage);

    client->Write("Hello there!!!\n");
}

void OnMessage(NetSocket::ClientConnection::pointer client, void *data, uint32_t length)
{
    std::string message = std::string((char*)data, length);
    std::cout << message << std::endl;
}