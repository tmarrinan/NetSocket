#ifndef __NETSOCKET_SERVER_H_
#define __NETSOCKET_SERVER_H_

#include <map>
#include "netsocket.h"
#include "netsocket/clientconnection.h"

class NetSocket::Server {
private:
    asio::io_service io_service;
    tcp::acceptor acceptor;
    std::map<std::string, ClientConnection::pointer> clients;
    void (*connect_callback)(ClientConnection::pointer client);

    void StartAccept();
    void HandleAccept(ClientConnection::pointer new_connection, const asio::error_code& error);
public:
    Server(uint16_t port);
    void Run();
    void ConnectCallback(void (*callback)(ClientConnection::pointer client));
};

#endif // __NETSOCKET_SERVER_H_