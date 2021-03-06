#include <iostream>
#include "netsocket.h"
#include "netsocket/client.h"

void OnConnect(NetSocket::Client& client);
void OnDisconnect(NetSocket::Client& client);
void OnStringMessage(NetSocket::Client& client, std::string message);
void OnBinaryMessage(NetSocket::Client& client, void *data, uint32_t length);

int main(int argc, char **argv)
{
    // create client
    NetSocket::ClientOptions options = NetSocket::CreateClientOptions();
    options.secure = false;
    options.flags = NetSocket::GeneralFlags::None; 
    //options.flags = NetSocket::ClientFlags::VerifyPeer;
    NetSocket::Client client("localhost", 8000, options);

    // add callback functions
    client.ConnectCallback(OnConnect);
    client.DisconnectCallback(OnDisconnect);
    client.ReceiveStringCallback(OnStringMessage);
    client.ReceiveBinaryCallback(OnBinaryMessage);

    // run indefinitey (quit if server closes connection)
    client.Run();

    return 0;
}

void OnConnect(NetSocket::Client& client)
{
    std::cout << "CONNECT to server" << std::endl;
}

void OnDisconnect(NetSocket::Client& client)
{
    std::cout << "DISCONNECT from server" << std::endl;
}

void OnStringMessage(NetSocket::Client& client, std::string message)
{
    std::cout << "String: " << message << std::endl;

    client.Send("I'm Alive!");
}

void OnBinaryMessage(NetSocket::Client& client, void *data, uint32_t length)
{
    uint8_t *message = (uint8_t*)data;
    std::cout << "Binary: 0x" << std::hex << static_cast<int>(message[0]) << std::dec << "... [" << length << " bytes]" << std::endl;

    uint8_t buffer[6] = {65, 118, 5, 192, 42, 220};
    client.Send(buffer, 6, NetSocket::CopyMode::MemCopy);
}