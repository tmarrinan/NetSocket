#include "netsocket/server.h"

NetSocket::Server::Server(uint16_t port) :
    acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
    connect_callback(NULL)
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
    ClientConnection::pointer new_connection = ClientConnection::Create(acceptor.get_io_service());
    acceptor.async_accept(new_connection->Socket(), std::bind(&Server::HandleAccept, this, new_connection, std::placeholders::_1));
}

void NetSocket::Server::HandleAccept(NetSocket::ClientConnection::pointer new_connection, const asio::error_code& error)
{
    if (!error)
    {
        clients[new_connection->Endpoint()] = new_connection;
        if (connect_callback) connect_callback(new_connection);

        new_connection->Read();
    }

    StartAccept();
}

void NetSocket::Server::ConnectCallback(void (*callback)(NetSocket::ClientConnection::pointer client))
{
    connect_callback = callback;
}