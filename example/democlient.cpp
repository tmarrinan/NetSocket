#include <iostream>
#include "netsocket/client.h"

void OnDisconnect(NetSocket::Client& client);
void OnStringMessage(NetSocket::Client& client, std::string message);
void OnBinaryMessage(NetSocket::Client& client, void *data, uint32_t length);

int main(int argc, char **argv)
{
    // create client
    NetSocket::Client client("localhost", 8000);

    // add callback functions
    client.DisconnectCallback(OnDisconnect);
    client.ReceiveStringCallback(OnStringMessage);
    client.ReceiveBinaryCallback(OnBinaryMessage);

    // run indefinitey
    client.Run();

    return 0;
}

void OnDisconnect(NetSocket::Client& client)
{
    std::cout << "DISCONNECT from server" << std::endl;
}

void OnStringMessage(NetSocket::Client& client, std::string message)
{
    std::cout << "String: " << message << std::endl;
}

void OnBinaryMessage(NetSocket::Client& client, void *data, uint32_t length)
{
    uint8_t *message = (uint8_t*)data;
    std::cout << "Binary: 0x" << std::hex << static_cast<int>(message[0]) << std::dec << "... [" << length << " bytes]" << std::endl;
}