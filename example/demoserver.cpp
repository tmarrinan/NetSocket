#include <iostream>
#include <string>
#include <map>
#include <chrono>
#include "netsocket.h"
#include "netsocket/server.h"

typedef struct TimeTest {
    uint64_t start_time;
    int send_count;
} TimeTest;

int main(int argc, char **argv)
{
    uint16_t port = 8000;
    uint32_t size = 4 * 1024 * 1024;
    uint8_t *buffer = new uint8_t[size];

    // create server options (encrypt?)
    NetSocket::ServerOptions options = NetSocket::CreateServerOptions();
    options.flags = NetSocket::GeneralFlags::None;
    options.send_buf_size = 131072;
    options.recv_buf_size = 131072;
    //options.server_cert = "../example/keys/server_crt_key.pem";
    //options.dh_file = "../example/keys/dh_2048.pem";
    //options.flags = NetSocket::ServerFlags::NoSslV2 | NetSocket::ServerFlags::NoSslV3 | NetSocket::ServerFlags::NoTlsV1 | NetSocket::ServerFlags::NoTlsV1_1;
    // create server
    NetSocket::Server server(port, options);
    
    // event loop
    int max = 100;
    std::map<std::string, TimeTest> timer;
    while (server.Alive())
    {
        NetSocket::Server::Event event = server.WaitForNextEvent();
        uint8_t* bin = reinterpret_cast<uint8_t*>(event.binary_data);
        switch (event.type)
        {
            case NetSocket::Server::EventType::Connect:
                std::cout << "New Connection: " << event.client->Endpoint() << std::endl;
                timer[event.client->Endpoint()] = {(uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(), 0};
                event.client->Send(buffer, size, NetSocket::CopyMode::ZeroCopy);
                break;
            case NetSocket::Server::EventType::Disconnect:
                std::cout << "Client Disconnect: " << event.client->Endpoint() << std::endl;
                break;
            case NetSocket::Server::EventType::SendFinished:
                //std::cout << "Finished send " << count << std::endl;
                break;
            case NetSocket::Server::EventType::ReceiveBinary:
                timer[event.client->Endpoint()].send_count++;
                if (timer[event.client->Endpoint()].send_count == max)
                {
                    uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                    uint32_t bits = size * max * 8;
                    uint32_t gig = 1024 * 1024 * 1024;
                    double sec = (double)(now - timer[event.client->Endpoint()].start_time) / 1000.0;
                    std::cout << event.client->Endpoint() << " send speed: " << ((double)bits / (double)gig) / sec << " Gbps" << std::endl;
                }
                else
                {
                    event.client->Send(buffer, size, NetSocket::CopyMode::ZeroCopy);
                }
                break;
            default:
                break;
        }
    }

    return 0;
}
