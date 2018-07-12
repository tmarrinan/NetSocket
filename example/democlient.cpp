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

    // event loop
    uint8_t buffer[6] = {65, 118, 5, 192, 42, 220};
    while (client.Alive())
    {
        NetSocket::Client::Event event = client.WaitForNextEvent();
        uint8_t* bin = reinterpret_cast<uint8_t*>(event.binary_data);
        switch (event.type)
        {
            case NetSocket::Client::EventType::Connect:
                std::cout << "CONNECT to server" << std::endl;
                break;
            case NetSocket::Client::EventType::Disconnect:
                std::cout << "DISCONNECT from server" << std::endl;
                break;
            case NetSocket::Client::EventType::ReceiveString:
                std::cout << "String: " << event.string_data << std::endl;
                client.Send("I'm Alive!");
                break;
            case NetSocket::Client::EventType::ReceiveBinary:
                std::cout << "Binary: 0x" << std::hex << static_cast<int>(bin[0]) << std::dec << "... [" << event.data_length << " bytes]" << std::endl;
                client.Send(buffer, 6, NetSocket::CopyMode::MemCopy);
                break;
            default:
                break;
        }
    }

    return 0;
}
