#include "netsocket/server.h"

NetSocket::Server::Server(uint16_t port) :
    acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
    connect_callback(NULL),
    disconnect_callback(NULL)
{
    StartAccept();
    std::cout << "[NetSocket::Server] Now listening at " << acceptor.local_endpoint() << std::endl;
}

void NetSocket::Server::Run()
{
    io_service.run();
}

void NetSocket::Server::Poll()
{
    io_service.poll();
}

void NetSocket::Server::StartAccept()
{
    ClientConnection::Pointer new_connection = ClientConnection::Create(this, acceptor.get_io_service());
    acceptor.async_accept(new_connection->Socket(), std::bind(&Server::HandleAccept, this, new_connection, std::placeholders::_1));
}

void NetSocket::Server::HandleAccept(NetSocket::ClientConnection::Pointer new_connection, const asio::error_code& error)
{
    if (!error)
    {
        new_connection->DisconnectCallback(std::bind(&Server::HandleDisconnect, this, std::placeholders::_1));

        clients[new_connection->Endpoint()] = new_connection;
        if (connect_callback) connect_callback(*this, new_connection);

        new_connection->Receive();
    }

    StartAccept();
}

void NetSocket::Server::HandleDisconnect(std::string endpoint)
{
    clients.erase(endpoint);
    if (disconnect_callback) disconnect_callback(*this, endpoint);
}

void NetSocket::Server::Broadcast(std::string message)
{
    for (auto const& it : clients)
    {
        it.second->Send(message);
    }
}

void NetSocket::Server::Broadcast(const void *message, uint32_t length, CopyMode mode)
{
    for (auto const& it : clients)
    {
        it.second->Send(message, length, mode);
    }
}

void NetSocket::Server::ConnectCallback(std::function<void(Server&, ClientConnection::Pointer)> callback)
{
    connect_callback = callback;
}

void NetSocket::Server::DisconnectCallback(std::function<void(Server&, std::string)> callback)
{
    disconnect_callback = callback;
}