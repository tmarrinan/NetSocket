#include <iostream>
#include "netsocket/server.h"

void OnConnection(NetSocket::Server& server, NetSocket::ClientConnection::Pointer client);
void OnDisonnect(NetSocket::Server& server, std::string client_endpoint);
void OnStringMessage(NetSocket::Server& server, NetSocket::ClientConnection::Pointer client, std::string message);
void OnBinaryMessage(NetSocket::Server& server, NetSocket::ClientConnection::Pointer client, void *data, uint32_t length);

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
    client->ReceiveStringCallback(OnStringMessage);
    client->ReceiveBinaryCallback(OnBinaryMessage);

    client->Send("Hello there!!!");
}

void OnDisonnect(NetSocket::Server& server, std::string client_endpoint)
{
    std::cout << "Client Disconnect: " << client_endpoint << std::endl;
    server.Broadcast("DISCONNECT --> " + client_endpoint);
}

void OnStringMessage(NetSocket::Server& server, NetSocket::ClientConnection::Pointer client, std::string message)
{
    std::cout << "String: " << message << std::endl;
}

void OnBinaryMessage(NetSocket::Server& server, NetSocket::ClientConnection::Pointer client, void *data, uint32_t length)
{
    uint8_t *message = (uint8_t*)data;
    std::cout << "Binary: 0x" << std::hex << static_cast<int>(message[0]) << std::dec << "... [" << length << " bytes]" << std::endl;
}