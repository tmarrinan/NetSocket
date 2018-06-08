#include "netsocket/server.h"

NetSocket::Server::Server(uint16_t port) :
    acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
    connect_callback(NULL),
    disconnect_callback(NULL)
{
    try
    {
        StartAccept();
        std::cout << "[NetSocket::Server] Now listening at " << acceptor.local_endpoint() << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void NetSocket::Server::Run()
{
    try
    {
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
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

        new_connection->Read();
    }

    StartAccept();
}

void NetSocket::Server::HandleDisconnect(std::string endpoint)
{
    clients.erase(endpoint);
    if (disconnect_callback) disconnect_callback(*this, endpoint);
}

void NetSocket::Server::ConnectCallback(std::function<void(Server&, ClientConnection::Pointer)> callback)
{
    connect_callback = callback;
}

void NetSocket::Server::DisconnectCallback(std::function<void(Server&, std::string)> callback)
{
    disconnect_callback = callback;
}