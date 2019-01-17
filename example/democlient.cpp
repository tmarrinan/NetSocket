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
    options.send_buf_size = 131072;
    options.recv_buf_size = 131072;
    //options.flags = NetSocket::ClientFlags::VerifyPeer;
    NetSocket::Client client("localhost", 8000, options);

    // event loop
    uint8_t receive = 1;
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
            case NetSocket::Client::EventType::ReceiveBinary:
                delete[] (uint8_t*)event.binary_data;
                client.Send(&receive, 1, NetSocket::CopyMode::ZeroCopy);
                break;
            default:
                break;
        }
    }

    return 0;
}
