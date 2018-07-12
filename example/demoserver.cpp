#include <iostream>
#include "netsocket.h"
#include "netsocket/server.h"

int main(int argc, char **argv)
{
    uint16_t port = 8000;
    uint8_t buffer[8] = {232, 65, 3, 16, 15, 255, 111, 192};

    // create server options (encrypt?)
    NetSocket::ServerOptions options = NetSocket::CreateServerOptions();
    options.flags = NetSocket::GeneralFlags::None;
    //options.server_cert = "../example/keys/server_crt_key.pem";
    //options.dh_file = "../example/keys/dh_2048.pem";
    //options.flags = NetSocket::ServerFlags::NoSslV2 | NetSocket::ServerFlags::NoSslV3 | NetSocket::ServerFlags::NoTlsV1 | NetSocket::ServerFlags::NoTlsV1_1;
    // create server
    NetSocket::Server server(port, options);
    
    // event loop
    while (server.Alive())
    {
        NetSocket::Server::Event event = server.WaitForNextEvent();
        uint8_t* bin = reinterpret_cast<uint8_t*>(event.binary_data);
        switch (event.type)
        {
            case NetSocket::Server::EventType::Connect:
                std::cout << "New Connection: " << event.client->Endpoint() << std::endl;
                event.client->Send("Hello there!!!");
                event.client->Send(buffer, 8, NetSocket::CopyMode::ZeroCopy);
                break;
            case NetSocket::Server::EventType::Disconnect:
                std::cout << "Client Disconnect: " << event.client->Endpoint() << std::endl;
                server.Broadcast("DISCONNECT --> " + event.client->Endpoint());
                break;
            case NetSocket::Server::EventType::ReceiveString:
                std::cout << "String: " << event.string_data << std::endl;
                break;
            case NetSocket::Server::EventType::ReceiveBinary:
                std::cout << "Binary: 0x" << std::hex << static_cast<int>(bin[0]) << std::dec << "... [" << event.data_length << " bytes]" << std::endl;
                break;
            default:
                break;
        }
    }

    return 0;
}
