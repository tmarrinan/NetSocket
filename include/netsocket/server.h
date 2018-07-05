#ifndef __NETSOCKET_SERVER_H_
#define __NETSOCKET_SERVER_H_

#include <map>
#include "netsocket.h"
#include "netsocket/clientconnection.h"

class NetSocket::Server {
private:
    asio::io_service io_service;
    tcp::acceptor acceptor;
    asio::ssl::context context;
    bool secure;
    std::map<std::string, ClientConnection::Pointer> clients;
    std::function<void(Server&, ClientConnection::Pointer)> connect_callback;
    std::function<void(Server&, std::string)> disconnect_callback;

    std::string GetPassword();
    void StartAccept();
    void HandleAccept(ClientConnection::Pointer new_connection, const asio::error_code& error);
    void HandleHandshake(NetSocket::ClientConnection::Pointer new_connection, const asio::error_code& error);
    void HandleDisconnect(std::string endpoint);
public:
    NETSOCKET_EXPORT Server(uint16_t port, const char *tls_cert, const char *dh);
    NETSOCKET_EXPORT void Run();
    NETSOCKET_EXPORT void Poll();
    NETSOCKET_EXPORT void Broadcast(std::string message);
    NETSOCKET_EXPORT void Broadcast(const void *message, uint32_t length, CopyMode mode);
    NETSOCKET_EXPORT void ConnectCallback(std::function<void(Server&, ClientConnection::Pointer)> callback);
    NETSOCKET_EXPORT void DisconnectCallback(std::function<void(Server&, std::string)> callback);
};

#endif // __NETSOCKET_SERVER_H_