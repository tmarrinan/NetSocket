#ifndef __NETSOCKET_SERVER_H_
#define __NETSOCKET_SERVER_H_

#include <map>
#include "netsocket.h"
#include "netsocket/clientconnection.h"

class NetSocket::Server {
private:
    asio::io_service io_service;
    tcp::acceptor acceptor;
    std::map<std::string, ClientConnection::Pointer> clients;
    std::function<void(Server&, ClientConnection::Pointer)> connect_callback;
    std::function<void(Server&, std::string)> disconnect_callback;

    void StartAccept();
    void HandleAccept(ClientConnection::Pointer new_connection, const asio::error_code& error);
    void HandleDisconnect(std::string endpoint);
public:
    Server(uint16_t port);
    void Run();
    void Broadcast(std::string message);
    void Broadcast(const void *message, uint32_t length);
    //void Broadcast(void *message, uint32_t length);
    void ConnectCallback(std::function<void(Server&, ClientConnection::Pointer)> callback);
    void DisconnectCallback(std::function<void(Server&, std::string)> callback);
};

#endif // __NETSOCKET_SERVER_H_